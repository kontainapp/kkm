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

#include <asm/paravirt.h>
#include "kkm_platform.h"

static uint64_t kkm_platfrom_pv_read_cr3(void)
{
	uint64_t value = 0;
#ifdef CONFIG_PARAVIRT_XXL
	value = __read_cr3();
#else
	__asm__ volatile("movq %%cr3, %0" : "=r"(value) : : "memory");
#endif
	return value;
}

static void kkm_platform_pv_write_cr3(uint64_t value)
{
#ifdef CONFIG_PARAVIRT_XXL
	write_cr3(value);
#else
	__asm__ volatile("movq %0, %%cr3" : : "r"(value) : "memory");
#endif
}

static uint64_t kkm_platfrom_pv_read_cr4(void)
{
	uint64_t value = 0;
	__asm__ volatile("movq %%cr4, %0" : "=r"(value) : : "memory");
	return value;
}

static void kkm_platform_pv_write_cr4(uint64_t value)
{
#ifdef CONFIG_PARAVIRT_XXL
	__write_cr4(value);
#else
	__asm__ volatile("movq %0, %%cr4" : : "r"(value) : "memory");
#endif
}

static void kkm_platform_pv_load_idt(const struct desc_ptr *dptr)
{
#ifdef CONFIG_PARAVIRT_XXL
	load_idt(dptr);
#else
	__asm__ volatile("lidt %0" : : "m"(*dptr));
#endif
}

static void kkm_platform_pv_store_idt(struct desc_ptr *dptr)
{
	__asm__ volatile("sidt %0" : "=m"(*dptr));
}

struct kkm_platform_calls kkm_platfrom_pv = {
	.kkm_read_cr3 = kkm_platfrom_pv_read_cr3,
	.kkm_write_cr3 = kkm_platform_pv_write_cr3,
	.kkm_read_cr4 = kkm_platfrom_pv_read_cr4,
	.kkm_write_cr4 = kkm_platform_pv_write_cr4,
	.kkm_load_idt = kkm_platform_pv_load_idt,
	.kkm_store_idt = kkm_platform_pv_store_idt,
};
