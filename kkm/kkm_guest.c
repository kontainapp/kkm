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
