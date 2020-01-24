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

#ifndef __KKM_KONTAINER_H__
#define __KKM_KONTAINER_H__

#define GUEST_PRIVATE_DATA_SIZE (512)
#define GUEST_STACK_REDZONE_SIZE (128)
#define GUEST_STACK_SIZE                                                       \
	(PAGE_SIZE - GUEST_PRIVATE_DATA_SIZE - GUEST_STACK_REDZONE_SIZE)

#define GUEST_STACK_START_ADDRESS(guest_area_start)                            \
	(guest_area_start + ((struct kkm_guest_area *)0)->redzone)

struct kkm_guest_area {
	// data store
	union {
		struct kkm *kkm;
		char reserved[GUEST_PRIVATE_DATA_SIZE];
	};
	char stack[GUEST_STACK_SIZE];
	char redzone[GUEST_STACK_REDZONE_SIZE];
};

int kkm_kontainer_init(struct kkm *kkm);
void kkm_kontainer_cleanup(struct kkm *kkm);

#endif /* __KKM_KONTAINER_H__ */
