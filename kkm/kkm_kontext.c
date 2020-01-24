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

int kkm_kontext_init(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	// stack0
	ret_val = kkm_mm_allocate_page(&kkm_kontext->stack_page,
				       &kkm_kontext->stack, NULL);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontext_init: Failed to allocate memory for stack0 error(%d)\n",
		       ret_val);
		goto error;
	}

	printk(KERN_NOTICE "kkm_kontext_init: stack0 page %lx va %p\n",
	       (unsigned long)kkm_kontext->stack_page, kkm_kontext->stack);

error:
	if (ret_val != 0) {
		kkm_kontext_cleanup(kkm_kontext);
	}
	return ret_val;
}

void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext)
{
	if (kkm_kontext->stack_page != NULL) {
		free_page((unsigned long long)kkm_kontext->stack);
		kkm_kontext->stack_page = NULL;
		kkm_kontext->stack = NULL;
	}
}

int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;

	printk(KERN_NOTICE "kkm_kontext_switch_kernel:\n");

	// save kernel address space
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: native kernel cr3 %lx\n",
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
