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

/*
 * PTI_USER_PGTABLE_MASK not visible to modules
 * make up our own mask
 */
#define KKM_USER_PGTABLE_BIT (PAGE_SHIFT)
#define KKM_USER_PGTABLE_MASK (1 << KKM_USER_PGTABLE_BIT)

int kkm_kontainer_init(struct kkm *kkm)
{
	int ret_val = 0;

	/*
	 * allocated pages for pml4
	 */
	ret_val = kkm_mm_allocate_pages(&kkm->guest_kernel_page,
					(void **)&kkm->guest_kernel_va,
					&kkm->guest_kernel_pa, 2);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_init: Failed to allocate memory for guest kernel page table error(%d)\n",
		       ret_val);
		goto error;
	}

	printk(KERN_NOTICE
	       "kkm_kontainer_init: guest kernel page %px va %lx pa %llx\n",
	       kkm->guest_kernel_page, kkm->guest_kernel_va,
	       kkm->guest_kernel_pa);

	if ((kkm->guest_kernel_va & KKM_USER_PGTABLE_MASK) ==
	    KKM_USER_PGTABLE_MASK) {
		printk(KERN_ERR
		       "kkm_kontainer_init: unexpected odd start page address\n");
		ret_val = -EINVAL;
		goto error;
	}

	/*
	 * allocate 2 pages,
	 *     kernel mode pml4
	 *     guest mode pml4
	 * kernel code depends on kernel page table at even page and user page table at next(odd) page
	 */
	kkm->guest_payload_va = kkm->guest_kernel_va + PAGE_SIZE;
	kkm->guest_payload_pa += kkm->guest_kernel_pa + PAGE_SIZE;

	printk(KERN_NOTICE
	       "kkm_kontainer_init: guest payload page %px va %lx pa %llx\n",
	       kkm->guest_payload_page, kkm->guest_payload_va,
	       kkm->guest_payload_pa);

error:
	if (ret_val != 0) {
		kkm_kontainer_cleanup(kkm);
	}
	return ret_val;
}

/*
 * cleanup
 */
void kkm_kontainer_cleanup(struct kkm *kkm)
{
	if (kkm->guest_kernel_page != NULL) {
		free_page(kkm->guest_kernel_va);
		kkm->guest_kernel_page = NULL;

		kkm->guest_kernel_va = 0;
		kkm->guest_kernel_pa = 0;

		kkm->guest_payload_va = 0;
		kkm->guest_payload_pa = 0;
	}
}
