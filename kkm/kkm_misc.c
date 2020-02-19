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

void kkm_idt_invalidate(void *address)
{
	struct desc_ptr kkm_idt;

	kkm_idt.size = 0;
	kkm_idt.address = (unsigned long)address;

	load_idt(&kkm_idt);
}
