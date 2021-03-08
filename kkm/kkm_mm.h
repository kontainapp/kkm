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

#ifndef __KKM_MM_H__
#define __KKM_MM_H__

int kkm_mm_allocate_pages(struct page **page, void **virtual_address,
			  phys_addr_t *physical_address, int count);
int kkm_mm_allocate_page(struct page **page, void **virtual_address,
			 phys_addr_t *physical_address);
void kkm_mm_free_pages(void *virtual_address, int count);
void kkm_mm_free_page(void *virtual_address);

#endif /* __KKM_MM_H__ */
