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
/*
#include <asm/proto.h>
#include <asm/desc.h>
#include <asm/hw_irq.h>
*/

#include "kkm.h"
#include "kkm_mm.h"
#include "kkm_guest.h"


int kkm_guest_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;

	printk(KERN_NOTICE "kkm_guest_switch_kernel\n");

	// save kernel address space
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	printk(KERN_NOTICE "native kernel cr3 %lx\n",
	       kkm_kontext->native_kernel_cr3);

	// change to guest kernel address space
	write_cr3(kkm->guest_kernel_pa);

	kkm_kontext->guest_kernel_cr3 = __read_cr3();

	// flush TLB from guest + payload
	write_cr3(kkm->guest_kernel_pa);

	// restore kernel address space
	write_cr3(kkm_kontext->native_kernel_cr3);

	return ret_val;
}

int kkm_guest_init(struct kkm *kkm)
{
	int ret_val = 0;
	int i = 0;
	gate_desc *gd = NULL;
	unsigned long long desc_addr = (unsigned long long)0;

	kkm->idt_page = alloc_pages(GFP_KERNEL | __GFP_ZERO, 1);
	if (kkm->idt_page == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}
	kkm->idt = (unsigned long)page_address(kkm->idt_page);
	gd = (gate_desc *)kkm->idt;
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

error:
	kkm_guest_cleanup(kkm);
	return ret_val;
}

void kkm_guest_cleanup(struct kkm *kkm)
{
	if (kkm->idt_page != NULL) {
		free_page(kkm->idt);
		kkm->idt_page = NULL;
		kkm->idt = 0;
	}
}
