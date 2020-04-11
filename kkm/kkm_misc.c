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

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_kontext.h"
#include "kkm_misc.h"

/*
 * dont use
 * this will cause triple exception.
 * as faults cannot be disabled.
 */
void kkm_idt_invalidate(void *address)
{
	struct desc_ptr kkm_idt;

	kkm_idt.size = 0;
	kkm_idt.address = (unsigned long)address;

	load_idt(&kkm_idt);
}

/*
 * use CR4 to flush tbl.
 * don't use __native_flush_tlb_global
 * __native_flush_tlb_global is causing return to user space
 */
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

/*
 * kernel uses ASID's and are managed by kernel
 * make sure tlb is completely cleared
 */
void kkm_change_address_space(phys_addr_t pgd_pa)
{
	/* change space */
	write_cr3(pgd_pa);

	/* flush TLB */
	kkm_flush_tlb_all();
}

/*
 * initialize redzone around guest kernel stack
 */
void kkm_init_guest_area_redzone(struct kkm_guest_area *ga)
{
	memset(ga->redzone_top, REDZONE_DATA, GUEST_STACK_REDZONE_SIZE);
	memset(ga->redzone_bottom, REDZONE_DATA, GUEST_STACK_REDZONE_SIZE);
}

/*
 * verify redzone around guest kernel stack
 */
void kkm_verify_guest_area_redzone(struct kkm_guest_area *ga)
{
	if (kkm_verify_bytes(ga->redzone_top, GUEST_STACK_REDZONE_SIZE,
			     REDZONE_DATA) == false) {
		printk(KERN_NOTICE
		       "kkm_verify_guest_area_redzone: top rezone mismatch\n");
	}
	if (kkm_verify_bytes(ga->redzone_bottom, GUEST_STACK_REDZONE_SIZE,
			     REDZONE_DATA) == false) {
		printk(KERN_NOTICE
		       "kkm_verify_guest_area_redzone: bottom rezone mismatch\n");
	}
}

/*
 * verify buffer for known pattern
 */
bool kkm_verify_bytes(uint8_t *data, uint32_t count, uint8_t value)
{
	int i = 0;
	bool ret_val = true;

	for (i = 0; i < count; i++) {
		if (data[i] == value) {
			continue;
		}
		ret_val = false;
		printk(KERN_NOTICE
		       "kkm_verify_bytes: data mismatch expected(0x%2x) found (0x%2x)\n",
		       REDZONE_DATA, data[i]);
		break;
	}
	return ret_val;
}

void kkm_show_trap_info(struct kkm_guest_area *ga)
{
	printk(KERN_NOTICE
	       "kkm_show_trap_info: thread %d ga %px cr2 %llx intr_no %llx error %llx rip %llx cs %llx rflags %llx rsp %llx ss %llx\n",
	       ga->kkm_kontext->kontext_fd, ga, ga->sregs.cr2, ga->kkm_intr_no,
	       ga->trap_info.error, ga->trap_info.rip, ga->trap_info.cs,
	       ga->trap_info.rflags, ga->trap_info.rsp, ga->trap_info.ss);
}
