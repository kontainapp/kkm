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
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_0 (0)
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_255 (255 * 8)
#define KKM_PGD_PAYLOAD_SIZE (8)

int kkm_mm_allocate_page(struct page **page, void **virtual_address,
			 phys_addr_t *physical_address)
{
	int ret_val = 0;

	if ((page == NULL) || (virtual_address == NULL)) {
		ret_val = -EINVAL;
		goto error;
	}

	*page = alloc_pages(GFP_KERNEL | __GFP_ZERO, 1);
	if (*page == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}
	*virtual_address = page_address(*page);
	if (physical_address != NULL) {
		*physical_address = virt_to_phys(*virtual_address);
	}

error:
	return ret_val;
}

static void kkm_mm_copy_range(unsigned long long src_base,
			      unsigned long long src_offset,
			      unsigned long long dest_base,
			      unsigned long long dest_offset, size_t count)
{
	memcpy((void *)dest_base + dest_offset, (void *)src_base + src_offset,
	       count);
}

int kkm_mm_copy_kernel_pgd(struct kkm *kkm)
{
	// when running in kernel mode we are expected to have kernel pgd
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;

	if (current_pgd_base == 0) {
		printk(KERN_NOTICE "kkm_mm_copy_kernel_pgd: PGD base is zero\n");
		return -EINVAL;
	}

	kkm_mm_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_kernel, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);

	// point to user pgd.
	// keep all memory map for now.
	// current_pgd_base += PAGE_SIZE;
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_payload, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);

	return 0;
}

int kkm_mm_sync(struct kkm *kkm)
{
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;

	// keep kernel and user pgd same for payload area
	// entry 0 for code+data
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_kernel, KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			  KKM_PGD_PAYLOAD_SIZE);
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_payload, KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			  KKM_PGD_PAYLOAD_SIZE);

	// entry 255 for stack+mmap
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_kernel, KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			  KKM_PGD_PAYLOAD_SIZE);
	kkm_mm_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_payload, KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			  KKM_PGD_PAYLOAD_SIZE);

	// fix memory alias created
	// modify km to use one pml4 entry for code + data and second entry for stack + mmap
	return 0;
}
