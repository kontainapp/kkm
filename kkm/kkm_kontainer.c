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
#include <linux/interrupt.h>
#include <asm/traps.h>
#include <asm/desc.h>

#include "kkm.h"
#include "kkm_kontext.h"
#include "kkm_kontainer.h"
#include "kkm_mm.h"
#include "kkm_entry.h"

// PTI_USER_PGTABLE_MASK not visible to modules
// make up our own
#define	KKM_USER_PGTABLE_BIT	(PAGE_SHIFT)
#define	KKM_USER_PGTABLE_MASK	(1 << KKM_USER_PGTABLE_BIT)

int kkm_kontainer_init(struct kkm *kkm)
{
	int ret_val = 0;
	int i = 0;
	gate_desc *gd = NULL;
	unsigned long long desc_addr = (unsigned long long)kkm_trap_entry;

	ret_val = kkm_mm_allocate_pages(&kkm->guest_kernel_page,
				       (void **)&kkm->guest_kernel,
				       &kkm->guest_kernel_pa, 2);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_init: Failed to allocate memory for guest kernel page table error(%d)\n",
		       ret_val);
		goto error;
	}

	printk(KERN_NOTICE
	       "kkm_kontainer_init: guest kernel page %lx va %lx pa %llx\n",
	       (unsigned long)kkm->guest_kernel_page, kkm->guest_kernel,
	       kkm->guest_kernel_pa);

	if ((kkm->guest_kernel & KKM_USER_PGTABLE_MASK) == KKM_USER_PGTABLE_MASK) {
		printk(KERN_ERR "kkm_kontainer_init: unexpected odd start page address\n");
		ret_val = -EINVAL;
		goto error;
	}

	// kernel code depends on kernel page table at even page and user page table at next(odd) page
	kkm->guest_payload = kkm->guest_kernel + PAGE_SIZE;
	kkm->guest_payload_pa += kkm->guest_kernel_pa + PAGE_SIZE;

	printk(KERN_NOTICE
	       "kkm_kontainer_init: guest payload page %lx va %lx pa %llx\n",
	       (unsigned long)kkm->guest_payload_page, kkm->guest_payload,
	       kkm->guest_payload_pa);

	ret_val = kkm_mm_allocate_page(&kkm->idt_page, &kkm->idt_va, NULL);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_init: Failed to allocate memory for idt error(%d)\n",
		       ret_val);
		goto error;
	}

	gd = (gate_desc *)kkm->idt_va;
	for (i = 0; i < IDT_ENTRIES; i++) {
		// TODO: setup different entry points based on normal kernel idt table
		// different entry point for each type
		gd[i].segment = __KERNEL_CS;

		gd[i].offset_low = desc_addr & 0xFFFF;
		gd[i].offset_middle = desc_addr >> 16 & 0xFFFF;
		gd[i].offset_high = desc_addr >> 32 & 0xFFFFFFFF;

		gd[i].bits.ist = 0;
		gd[i].bits.zero = 0;
		gd[i].bits.type = GATE_INTERRUPT;
		gd[i].bits.dpl = 0;
		gd[i].bits.p = 1;
	}

#if 0
	// store_gdt not available in PV(aws) kernels
	store_gdt(&kkm->native_gdt_descr);
	printk(KERN_NOTICE "kkm_kontainer_init: native kernel gdt size %x base %lx\n",
	       kkm->native_gdt_descr.size, kkm->native_gdt_descr.address);
#endif

	store_idt(&kkm->native_idt_descr);
	printk(KERN_NOTICE "kkm_kontainer_init: native kernel idt size %x base %lx\n",
	       kkm->native_idt_descr.size, kkm->native_idt_descr.address);
	if (kkm->native_idt_descr.size != (PAGE_SIZE - 1)) {
		printk(KERN_NOTICE "kkm_kontainer_init: idt size expecting 0xfff found %x\n",
				kkm->native_idt_descr.size);
	}

	memcpy(kkm->idt_va, (void *)kkm->native_idt_descr.address, PAGE_SIZE);

	kkm->guest_idt_descr.size = kkm->native_idt_descr.size;
	kkm->guest_idt_descr.address = (unsigned long)kkm->idt_va;


error:
	if (ret_val != 0) {
		kkm_kontainer_cleanup(kkm);
	}
	return ret_val;
}

void kkm_kontainer_cleanup(struct kkm *kkm)
{
	if (kkm->guest_kernel_page != NULL) {
		free_page(kkm->guest_kernel);
		kkm->guest_kernel_page = NULL;

		kkm->guest_kernel = 0;
		kkm->guest_kernel_pa = 0;

		kkm->guest_payload = 0;
		kkm->guest_payload_pa = 0;
	}
	if (kkm->idt_page != NULL) {
		free_page((unsigned long long)kkm->idt_va);
		kkm->idt_page = NULL;
		kkm->idt_va = NULL;
	}
}
