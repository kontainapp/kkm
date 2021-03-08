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

#ifndef __KKM_GUEST_ENTRY_H__
#define __KKM_GUEST_ENTRY_H__

void kkm_switch_to_gk_asm(struct kkm_guest_area *ga, uint64_t stack);
void kkm_switch_to_hk_asm(uint64_t nativer_kernel_cr3, uint64_t stack);
void kkm_switch_to_gp_asm(struct kkm_guest_area *ga);
void kkm_guest_entry_end(void);

#endif /* __KKM_GUEST_ENTRY_H__ */
