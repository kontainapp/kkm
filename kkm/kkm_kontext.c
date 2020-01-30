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

#include "kkm.h"
#include "kkm_kontext.h"
#include "kkm_mm.h"
#include "kkm_entry.h"

int kkm_kontext_init(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	// stack0
	ret_val = kkm_mm_allocate_page(&kkm_kontext->guest_area_page,
				       &kkm_kontext->guest_area, NULL);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontext_init: Failed to allocate memory for stack0 error(%d)\n",
		       ret_val);
		goto error;
	}

	printk(KERN_NOTICE "kkm_kontext_init: stack0 page %lx va %p\n",
	       (unsigned long)kkm_kontext->guest_area_page,
	       kkm_kontext->guest_area);

error:
	if (ret_val != 0) {
		kkm_kontext_cleanup(kkm_kontext);
	}
	return ret_val;
}

void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext)
{
	if (kkm_kontext->guest_area_page != NULL) {
		free_page((unsigned long long)kkm_kontext->guest_area);
		kkm_kontext->guest_area_page = NULL;
		kkm_kontext->guest_area = NULL;
	}
}

int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;
	struct kkm_guest_area *ga =
		(struct kkm_guest_area *)kkm_kontext->guest_area;
#if 1
	// delete
	uint64_t fsbase = 0;
	uint64_t gsbase = 0;
	uint64_t gskernelbase = 0;
	uint64_t efer = 0;
	uint64_t star = 0;

	rdmsrl(MSR_FS_BASE, fsbase);
	rdmsrl(MSR_GS_BASE, gsbase);
	rdmsrl(MSR_KERNEL_GS_BASE, gskernelbase);
	rdmsrl(MSR_EFER, efer);
	rdmsrl(MSR_STAR, star);

	printk(KERN_NOTICE "FSBASE %llx GSBASE %llx KERNGSBASE %llx\n", fsbase, gsbase, gskernelbase);
	printk(KERN_NOTICE "EFER %llx STAR %llx\n", efer, star);
#endif

	printk(KERN_NOTICE "kkm_kontext_switch_kernel:\n");

	memset(ga->redzone, 0xa5, GUEST_STACK_REDZONE_SIZE);
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: before %llx %llx %llx %llx\n",
	       (unsigned long long)ga->kkm, ga->guest_area_beg,
	       ga->native_kernel_stack, ga->guest_stack_variable_address);

	// save native kernel address space
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	kkm_kontext->native_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: native kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->native_kernel_cr3, kkm_kontext->native_kernel_cr4);

	// flush TLB, and disable PCID
	__write_cr4(kkm_kontext->native_kernel_cr4 & ~X86_CR4_PCIDE);

	// change to guest kernel address space
	write_cr3(kkm->guest_kernel_pa);

	kkm_kontext->guest_kernel_cr3 = __read_cr3();
	kkm_kontext->guest_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: guest kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->guest_kernel_cr3, kkm_kontext->guest_kernel_cr4);

	kkm_switch_to_guest(ga, kkm, (unsigned long long)ga->redzone);

	// flush TLB, and restore original cr4
	__write_cr4(kkm_kontext->native_kernel_cr4);

	// restore kernel address space
	write_cr3(kkm_kontext->native_kernel_cr3);

	printk(KERN_NOTICE "kkm_kontext_switch_kernel: after %llx %llx %llx %llx\n",
	       (unsigned long long)ga->kkm, ga->guest_area_beg,
	       ga->native_kernel_stack, ga->guest_stack_variable_address);

	return ret_val;
}

void kkm_guest_kernel_start(struct kkm_guest_area *ga)
{
	int value = 0x66;
	ga->guest_stack_variable_address = (unsigned long long)&value;

	kkm_switch_to_host(ga);
}
