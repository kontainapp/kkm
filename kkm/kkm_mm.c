// SPDX-License-Identifier: GPL-2.0
/*
 * Kontain Kernel Module
 *
 * This module enables Kontain unikernel in absence of
 * hardware support for virtualization
 *
 * Copyright (C) 2020-2021 Kontain Inc.
 *
 * Authors:
 *  Srinivasa Vetsa <svetsa@kontain.app>
 *
 */

#include <linux/mm.h>
#include <linux/log2.h>
#include <asm/io.h>

#include "kkm.h"
#include "kkm_mm.h"

/*
 * allocate multiple kernel page's
 * kernel interface allocates pages only in power of 2
 * if requested count is not power of 2 rest of the pages are unused
 *
 * return allocated page address, kernel virtual address of the page and physical address
 */
int kkm_mm_allocate_pages(struct page **page, void **virtual_address,
			  phys_addr_t *physical_address, int count)
{
	int ret_val = 0;
	int pow2count = 0;

	if (count == 0) {
		ret_val = -EINVAL;
		goto error;
	}
	pow2count = order_base_2(count); /* convert page count to power of 2 */

	if ((page == NULL) || (virtual_address == NULL)) {
		ret_val = -EINVAL;
		goto error;
	}

	/* allocate pages */
	*page = alloc_pages(GFP_KERNEL | __GFP_ZERO, pow2count);
	if (*page == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}

	/* get kernel virtual address from page pointer */
	*virtual_address = page_address(*page);

	if (physical_address != NULL) {
		/* get physical address if requested */
		*physical_address = virt_to_phys(*virtual_address);
	}

error:
	return ret_val;
}

/*
 * allocate one kernel page
 * return allocated page address, kernel virtual address of the page and physical address
 */
int kkm_mm_allocate_page(struct page **page, void **virtual_address,
			 phys_addr_t *physical_address)
{
	return kkm_mm_allocate_pages(page, virtual_address, physical_address,
				     1);
}

void kkm_mm_free_pages(void *virtual_address, int count)
{
	int pow2count = 0;

	if (count == 0) {
		return;
	}
	pow2count = order_base_2(count); /* convert page count to power of 2 */

	free_pages((uint64_t)virtual_address, pow2count);
}

void kkm_mm_free_page(void *virtual_address)
{
	kkm_mm_free_pages(virtual_address, 1);
}
