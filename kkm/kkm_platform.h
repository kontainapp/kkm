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
