/*
 * Copyright © 2020-2020 Kontain Inc. All rights reserved.
 *
 * Kontain Inc CONFIDENTIAL
 *
 * This file includes unpublished proprietary source code of Kontain Inc. The
 * copyright notice above does not evidence any actual or intended publication
 * of such source code. Disclosure of this source code or any related
 * proprietary information is strictly prohibited without the express written
 * permission of Kontain Inc.
 */

#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/desc.h>

#include "kkm.h"
#include "kkm_mm.h"

struct kkm_idt_entry {
	bool inited;
	struct page *idt_page;
	void *idt_va;
	struct desc_ptr guest_idt_desc;

	struct desc_ptr native_idt_desc;
};

struct kkm_idt_cache {
	int n_entries;

	struct kkm_idt_entry entries[0];
};

struct kkm_idt_cache *kkm_idt_cache = NULL;

int kkm_idt_cache_init(void)
{
	int ret_val = 0;
	int max_cpu_count = NR_CPUS;
	size_t alloc_size = 0;
	int i = 0;

	alloc_size = sizeof(struct kkm_idt_cache) +
		     sizeof(struct kkm_idt_entry) * max_cpu_count;
	printk(KERN_NOTICE "kkm_idt_cache_init: NR_CPUS %d size %ld\n",
	       max_cpu_count, alloc_size);
	kkm_idt_cache = (struct kkm_idt_cache *)kzalloc(alloc_size, GFP_KERNEL);
	if (kkm_idt_cache == NULL) {
		printk(KERN_NOTICE
		       "kkm_idt_cache_init: kmalloc returned NUL\n");
		ret_val = -ENOMEM;
		goto error;
	}
	kkm_idt_cache->n_entries = max_cpu_count;
	for (i = 0; i < kkm_idt_cache->n_entries; i++) {
		kkm_idt_cache->entries[i].inited = false;
	}

error:
	return ret_val;
}

void kkm_idt_cache_cleanup(void)
{
	int i = 0;
	struct kkm_idt_entry *entry;

	for (i = 0; i < kkm_idt_cache->n_entries; i++) {
		entry = &kkm_idt_cache->entries[i];
		if (entry->inited == true) {
			free_page((unsigned long long)entry->idt_va);
			entry->idt_page = NULL;
			entry->idt_va = NULL;
			entry->inited = false;
		}
	}
	kfree(kkm_idt_cache);
	kkm_idt_cache = NULL;
}

int kkm_idt_get_desc(struct desc_ptr **native_desc, struct desc_ptr **guest_desc)
{
	int ret_val = 0;
	int cpu = -1;
	struct kkm_idt_entry *entry;

	cpu = get_cpu();
	if (cpu >= kkm_idt_cache->n_entries) {
		printk(KERN_NOTICE
		       "kkm_idt_get_desc: current cpu id(%d) is greater than max cpus(%d)\n",
		       cpu, kkm_idt_cache->n_entries);
		ret_val = -EINVAL;
		goto error;
	}

	entry = &kkm_idt_cache->entries[cpu];

	if (entry->inited == false) {
		ret_val = kkm_mm_allocate_page(&entry->idt_page, &entry->idt_va,
					       NULL);
		if (ret_val != 0) {
			printk(KERN_NOTICE
			       "kkm_idt_get_desc: Failed to allocate memory for idt error(%d) on cpu(%d)\n",
			       ret_val, cpu);
			goto error;
		}
		printk(KERN_NOTICE "kkm_idt_get_desc: idt page %lx va %lx\n",
		       (unsigned long)entry->idt_page,
		       (unsigned long)entry->idt_va);

		store_idt(&entry->native_idt_desc);

		printk(KERN_NOTICE
		       "kkm_idt_get_desc: native kernel idt size %x base address %lx\n",
		       entry->native_idt_desc.size,
		       entry->native_idt_desc.address);
		if (entry->native_idt_desc.size != (PAGE_SIZE - 1)) {
			printk(KERN_NOTICE
			       "kkm_idt_get_desc: idt size expecting 0xfff found %x\n",
			       entry->native_idt_desc.size);
		}

		memcpy(entry->idt_va, (void *)entry->native_idt_desc.address,
		       PAGE_SIZE);

		entry->guest_idt_desc.size = entry->native_idt_desc.size;
		entry->guest_idt_desc.address = (unsigned long)entry->idt_va;

		printk(KERN_NOTICE
		       "kkm_idt_get_desc: guest kernel idt size %x base address %lx\n",
		       entry->guest_idt_desc.size,
		       entry->guest_idt_desc.address);

		// replace needed idt entries

		entry->inited = true;
	}

	*native_desc = &entry->native_idt_desc;
	*guest_desc = &entry->guest_idt_desc;

error:
	return ret_val;
}
