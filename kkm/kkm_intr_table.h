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

#ifndef __KKM_INTR_TABLE_H__
#define __KKM_INTR_TABLE_H__

extern uint64_t intr_function_pointers[NR_VECTORS];
extern uint64_t intr_forward_pointers[NR_VECTORS];

#endif /* __KKM_INTR_TABLE_H__ */
