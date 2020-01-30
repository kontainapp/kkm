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

#ifndef __KKM_KONTEXT_H__
#define __KKM_KONTEXT_H__

#define GUEST_PRIVATE_DATA_SIZE (1024)
#define GUEST_STACK_REDZONE_SIZE (256)
#define GUEST_STACK_SIZE                                                       \
	(PAGE_SIZE - GUEST_PRIVATE_DATA_SIZE - GUEST_STACK_REDZONE_SIZE)

#define GUEST_STACK_START_ADDRESS(guest_area_start)                            \
	(guest_area_start + ((struct kkm_guest_area *)0)->redzone)

struct kkm_guest_area {
	// data store
	union {
		struct {
			struct kkm *kkm;
			uint64_t guest_area_beg;
			uint64_t native_kernel_stack;
			uint64_t guest_stack_variable_address;

			struct kkm_regs regs;
			struct kkm_sregs sregs;
			struct kkm_debug debug;
			struct kkm_fpu fpu;
		};
		char data[GUEST_PRIVATE_DATA_SIZE];
	};
	char stack[GUEST_STACK_SIZE];
	char redzone[GUEST_STACK_REDZONE_SIZE];
};

int kkm_kontext_init(struct kkm_kontext *kkm_kontext);
void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext);
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext);
void kkm_guest_kernel_start(struct kkm_guest_area *ga);

#endif /* KKM_KONTEXT_H__ */
