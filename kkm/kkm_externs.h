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

#define	KKM_KM_GUEST_PRIVATE_MEM_START_VA	(512 * KKM_GIB)

#endif /* __KKM_EXTERNS_H__ */
