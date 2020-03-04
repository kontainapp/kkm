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
#include "kkm_mmu.h"
#include "kkm_intr.h"

/*
 * There is one idt system wide.
 * save the native kernel idt descriptor here
 * create kx idt and descriptor
 */
struct kkm_idt_entry {
	/*
	 * kx idt page and virtual address
	 */
	struct page *idt_page;
	void *idt_va;
	phys_addr_t idt_pa;

	void *idt_text_va;
	phys_addr_t idt_text_page0_pa;
	phys_addr_t idt_text_page1_pa;

	void *kx_global_va;
	phys_addr_t kx_global_pa;

	/*
	 * save native kernel idt descriptor
	 */
	struct desc_ptr native_idt_desc;

	/*
	 * kx idt descriptor
	 */
	struct desc_ptr guest_idt_desc;
};

/*
 * save descriptors that are changed when we kx enter
 * kernel maintains separate copy of these descriptors in cea
 */
struct kkm_desc_entry {
	bool inited;

	/*
	 * native kernel tss
	 */
	uint64_t native_tss_reg;
};

struct kkm_idt_cache {
	int n_entries;

	struct kkm_idt_entry idt_entry;

	struct kkm_desc_entry desc_entries[NR_CPUS];
};

struct kkm_idt_cache *kkm_idt_cache = NULL;

int kkm_idt_descr_init(void)
{
	int ret_val = 0;
	struct kkm_idt_entry *idt_entry;
	int i = 0;
	struct gate_struct *gs;
	uint64_t intr_entry_addr = 0;

	idt_entry = &kkm_idt_cache->idt_entry;

	/*
	 * allocate KKM_IDT_ALLOCATION_PAGES pages
	 */
	ret_val = kkm_mm_allocate_pages(&idt_entry->idt_page,
					&idt_entry->idt_va, &idt_entry->idt_pa,
					KKM_IDT_ALLOCATION_PAGES);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_idt_descr_init: Failed to allocate memory for idt error(%d)\n",
		       ret_val);
		goto error;
	}

	/*
	 * covneniece variables to track various addresses
	 */
	idt_entry->idt_text_va = idt_entry->idt_va + KKM_IDT_SIZE;
	idt_entry->idt_text_page0_pa = virt_to_phys(idt_entry->idt_text_va);
	idt_entry->idt_text_page1_pa =
		virt_to_phys(idt_entry->idt_text_va + PAGE_SIZE);

	idt_entry->kx_global_va = idt_entry->idt_text_va + KKM_IDT_CODE_SIZE;
	idt_entry->kx_global_pa = virt_to_phys(idt_entry->kx_global_va);

	printk(KERN_NOTICE
	       "kkm_idt_descr_init: idt page %px va %px pa %llx "
	       "intr entry va %px pa0 %llx pa1 %llx kx global va %px pa %llx\n",
	       idt_entry->idt_page, idt_entry->idt_va, idt_entry->idt_pa,
	       idt_entry->idt_text_va, idt_entry->idt_text_page0_pa,
	       idt_entry->idt_text_page1_pa, idt_entry->kx_global_va,
	       idt_entry->kx_global_pa);

	/*
	 * insert idt page, idt text and kx global in kx area
	 * idt in kx area is readonly
	 */
	kkm_mmu_set_idt(idt_entry->idt_pa);
	kkm_mmu_set_idt_text(idt_entry->idt_text_page0_pa,
			     idt_entry->idt_text_page1_pa);
	kkm_mmu_set_kx_global(idt_entry->kx_global_pa);

	/*
	 * save native kernel idt descriptor
	 */
	store_idt(&idt_entry->native_idt_desc);

	printk(KERN_NOTICE
	       "kkm_idt_descr_init: native kernel idt size %x base address %lx\n",
	       idt_entry->native_idt_desc.size,
	       idt_entry->native_idt_desc.address);
	if (idt_entry->native_idt_desc.size != (PAGE_SIZE - 1)) {
		printk(KERN_NOTICE
		       "kkm_idt_descr_init: idt size expecting 0xfff found %x\n",
		       idt_entry->native_idt_desc.size);
	}

	/*
	 * initialize idt entries
	 * use kva to initialize idt, kx idt page is readonly
	 */
	gs = (struct gate_struct *)idt_entry->idt_va;
	for (i = 0; i < NR_VECTORS; i++) {
		intr_entry_addr =
			KKM_IDT_CODE_START_VA + KKM_IDT_ENTRY_FUNCTION_SIZE * i;

		gs[i].offset_low = intr_entry_addr & 0xFFFF;
		gs[i].segment = __KERNEL_CS;
		gs[i].bits.ist = 0;
		gs[i].bits.zero = 0;
		gs[i].bits.type = GATE_INTERRUPT;
		gs[i].bits.dpl = 0;
		gs[i].bits.p = 1;
		gs[i].offset_middle = (intr_entry_addr >> 16) & 0xFFFF;
		gs[i].offset_high = (intr_entry_addr >> 32) & 0xFFFFFFFF;
		gs[i].reserved = 0;
	}

	idt_entry->guest_idt_desc.size = idt_entry->native_idt_desc.size;
	/*
	 * use kx address mapping for kx idt
	 */
	idt_entry->guest_idt_desc.address = (unsigned long)kkm_mmu_get_idt_va();

	printk(KERN_NOTICE
	       "kkm_idt_descr_init: guest kernel idt size %x base address %lx\n",
	       idt_entry->guest_idt_desc.size,
	       idt_entry->guest_idt_desc.address);

	/*
	 * copy interrupt entry code to kx area
	 */
	memcpy(idt_entry->idt_text_va, kkm_intr_start, KKM_IDT_CODE_SIZE);

	/*
	 * clear kx global area
	 */
	memset(idt_entry->kx_global_va, 0, KKM_IDT_GLOBAL_SIZE);

	// replace needed idt entries

error:
	return ret_val;
}

int kkm_idt_cache_init(void)
{
	int ret_val = 0;
	int i = 0;

	kkm_idt_cache = kzalloc(sizeof(struct kkm_idt_cache), GFP_KERNEL);
	if (kkm_idt_cache == NULL) {
		printk(KERN_NOTICE
		       "kkm_idt_cache_init: kmalloc returned NULL\n");
		ret_val = -ENOMEM;
		goto error;
	}

	kkm_idt_cache->n_entries = NR_CPUS;
	for (i = 0; i < NR_CPUS; i++) {
		kkm_idt_cache->desc_entries[i].inited = false;
	}

	if (kkm_idt_descr_init() != 0) {
		printk(KERN_NOTICE
		       "kkm_idt_cache_init: failed to initialize idt\n");
		goto error;
	}

error:
	return ret_val;
}

void kkm_idt_cache_cleanup(void)
{
	struct kkm_idt_entry *idt_entry;

	idt_entry = &kkm_idt_cache->idt_entry;
	free_page((unsigned long long)idt_entry->idt_va);
	idt_entry->idt_page = NULL;
	idt_entry->idt_va = NULL;

	kfree(kkm_idt_cache);
	kkm_idt_cache = NULL;
}

int kkm_idt_get_desc(struct desc_ptr *native_desc, struct desc_ptr *guest_desc)
{
	struct kkm_idt_entry *idt_entry;

	idt_entry = &kkm_idt_cache->idt_entry;

	native_desc->size = idt_entry->native_idt_desc.size;
	native_desc->address = idt_entry->native_idt_desc.address;

	guest_desc->size = idt_entry->guest_idt_desc.size;
	guest_desc->address = idt_entry->guest_idt_desc.address;

	return 0;
}
