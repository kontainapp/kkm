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

#ifndef __KKM_EXTERNS_H__
#define __KKM_EXTERNS_H__
/*
 * folowing is copied from km_mem.h
 * need to be kept in sync with monitor
 */
#define KKM_MIB (0x100000ULL)
#define KKM_GIB (0x40000000ULL)
#define KKM_TIB (0x10000000000ULL)

#define KKM_KM_RSRV_VDSOSLOT (41)
#define KKM_KM_RSRV_KMGUESTMEM_SLOT (42)

/*
 * bottom portion of guest address space
 */
#define KKM_GUEST_MEM_START_VA (2 * KKM_MIB)
#define KKM_GUEST_MAX_PHYS_MEM (512 * KKM_GIB)

/*
 * top portion of guest address space
 */
#define KKM_GUEST_MEM_TOP_VA (128 * KKM_TIB - 2 * KKM_MIB)
#define KKM_GUEST_VA_OFFSET                                                    \
	(KKM_GUEST_MEM_TOP_VA - (KKM_GUEST_MAX_PHYS_MEM - 2 * KKM_MIB))

/*
 * monitor mapping area for guest physical memory
 */
#define KKM_KM_USER_MEM_BASE                                                   \
	(0x100000000000ULL) /* keep in sync with KM_USER_MEM_BASE */

#define KKM_KM_GUEST_PRIVATE_MEM_START_VA (512 * KKM_GIB)

/*
 * VDSO/VVAR related macros == 0x8000000000
 */
#define KKM_GUEST_VVAR_VDSO_BASE_VA (KKM_KM_GUEST_PRIVATE_MEM_START_VA)

/*
 * == 0x8000008000
 */
#define KKM_GUEST_KMGUESTMEM_BASE_VA                                           \
	(KKM_KM_GUEST_PRIVATE_MEM_START_VA + 0x8000)

/*
 * keep in sync with km_hcalls.h:km_hc_args
 */
struct kkm_hc_args {
	uint64_t ret_val;
	uint64_t argument1;
	uint64_t argument2;
	uint64_t argument3;
	uint64_t argument4;
	uint64_t argument5;
	uint64_t argument6;
};
static_assert(sizeof(struct kkm_hc_args) == 56, "kkm_hc_args is known to monitor, size is fixed at 56 bytes");

#define KKM_KM_HC_ARGS_SIZE (sizeof(struct kkm_hc_args))

#endif /* __KKM_EXTERNS_H__ */
