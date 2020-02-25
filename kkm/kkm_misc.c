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
#include <asm/desc.h>
#include <asm/tlbflush.h>
#include <asm/debugreg.h>
#include <asm/cpu_entry_area.h>

// this will cause triple exception.
// as faults cannot be disabled.
void kkm_idt_invalidate(void *address)
{
	struct desc_ptr kkm_idt;

	kkm_idt.size = 0;
	kkm_idt.address = (unsigned long)address;

	load_idt(&kkm_idt);
}

// use CR4 to flush tbl.
// don't use __native_flush_tlb_global
// __native_flush_tlb_global is causing return to user space
void kkm_flush_tlb_all(void)
{
	unsigned long cr4;
	unsigned long flags;

	raw_local_irq_save(flags);
	cr4 = this_cpu_read(cpu_tlbstate.cr4);
	// toggle enable global pages
	native_write_cr4(cr4 ^ X86_CR4_PGE);
	// restore original cr4
	native_write_cr4(cr4);
	raw_local_irq_restore(flags);
}
