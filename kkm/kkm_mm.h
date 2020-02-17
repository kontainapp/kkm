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

#ifndef __KKM_MM_H__
#define __KKM_MM_H__

int kkm_mm_allocate_pages(struct page **page, void **virtual_address,
			 phys_addr_t *physical_address, int count);
int kkm_mm_allocate_page(struct page **page, void **virtual_address,
			 phys_addr_t *physical_address);
int kkm_mm_copy_kernel_pgd(struct kkm *kkm);
int kkm_mm_sync(struct kkm *kkm);

#endif /* __KKM_MM_H__ */
