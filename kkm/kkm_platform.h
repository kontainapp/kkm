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

#ifndef __KKM_PLATFORM_H__
#define __KKM_PLATFORM_H__

#include <linux/types.h>
#include <asm/desc_defs.h>

struct kkm_platform_calls {
	uint64_t (*kkm_read_cr3)(void);
	void (*kkm_write_cr3)(uint64_t value);
	uint64_t (*kkm_read_cr4)(void);
	void (*kkm_write_cr4)(uint64_t value);
	void (*kkm_load_idt)(const struct desc_ptr *dptr);
	void (*kkm_store_idt)(struct desc_ptr *dptr);
};

extern struct kkm_platform_calls kkm_platfrom_native;
extern struct kkm_platform_calls kkm_platfrom_pv;

#endif /* __KKM_PLATFORM_H__ */
