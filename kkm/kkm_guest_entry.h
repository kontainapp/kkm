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

#ifndef __KKM_GUEST_ENTRY_H__
#define __KKM_GUEST_ENTRY_H__

void kkm_switch_to_gk_asm(struct kkm_guest_area *ga, uint64_t stack);
void kkm_switch_to_hk_asm(uint64_t nativer_kernel_cr3, uint64_t stack);
void kkm_switch_to_gp_asm(struct kkm_guest_area *ga);
void kkm_guest_entry_end(void);

#endif /* __KKM_GUEST_ENTRY_H__ */
