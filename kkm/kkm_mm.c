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

#define KKM_PGD_KERNEL_OFFSET (2048)
#define KKM_PGD_KERNEL_SIZE (2048)

#define KKM_PGD_MONITOR_PAYLOAD_OFFSET (256)
#define KKM_PGD_GUEST_PAYLOAD_OFFSET (0)
#define KKM_PGD_PAYLOAD_SIZE (8)

void kkm_mm_copy_range(unsigned long long src_base,
		       unsigned long long src_offset,
		       unsigned long long dest_base,
		       unsigned long long dest_offset, size_t count)
{
	memcpy((void *)dest_base + dest_offset, (void *)src_base + src_offset,
	       count);
}

void kkm_mm_copy_kernel_pgd(struct kkm *kkm)
{
	// when running in kernel mode we are expected to have kernel pgd
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;

	if (current_pgd_base == 0) {
		printk(KERN_NOTICE "PGD base is zero\n");
		return;
	}

	kkm_mm_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_kernel, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);

	// point to user pgd
	current_pgd_base += PAGE_SIZE;
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_payload, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);
}

int kkm_mm_init(struct kkm *kkm)
{
	int ret_val = 0;

	kkm->guest_kernel_page = alloc_pages(GFP_KERNEL | __GFP_ZERO, 1);
	if (kkm->guest_kernel_page == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}
	kkm->guest_kernel = (unsigned long)page_address(kkm->guest_kernel_page);
	kkm->guest_kernel_pa = virt_to_phys((void *)kkm->guest_kernel);

	printk(KERN_NOTICE "kkm_mm_init guest kernel page %lx va %lx pa %llx\n",
	       (unsigned long)kkm->guest_kernel_page, kkm->guest_kernel,
	       kkm->guest_kernel_pa);

	kkm->guest_payload_page = alloc_page(GFP_KERNEL | __GFP_ZERO);
	if (kkm->guest_payload_page == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}
	kkm->guest_payload =
		(unsigned long)page_address(kkm->guest_payload_page);
	kkm->guest_payload_pa = virt_to_phys((void *)kkm->guest_payload);

	printk(KERN_NOTICE
	       "kkm_mm_init guest payload page %lx va %lx pa %llx\n",
	       (unsigned long)kkm->guest_payload_page, kkm->guest_payload,
	       kkm->guest_payload_pa);

	kkm_mm_copy_kernel_pgd(kkm);
error:
	if (ret_val != 0) {
		kkm_mm_cleanup(kkm);
	}
	return ret_val;
}

void kkm_mm_cleanup(struct kkm *kkm)
{
	if (kkm->guest_kernel_page != NULL) {
		free_page(kkm->guest_kernel);
		kkm->guest_kernel_page = NULL;
		kkm->guest_kernel = 0;
	}
	if (kkm->guest_payload_page != NULL) {
		free_page(kkm->guest_payload);
		kkm->guest_payload_page = NULL;
		kkm->guest_payload = 0;
	}
}

int kkm_mm_sync(struct kkm *kkm)
{
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;

	// keep kernel and user pgd same for payload area
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_kernel, KKM_PGD_GUEST_PAYLOAD_OFFSET,
			  KKM_PGD_PAYLOAD_SIZE);
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_payload, KKM_PGD_GUEST_PAYLOAD_OFFSET,
			  KKM_PGD_PAYLOAD_SIZE);
	return 0;
}
