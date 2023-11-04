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

#ifndef __KKM_MISC_H__
#define __KKM_MISC_H__

void kkm_idt_invalidate(void *address);
void kkm_flush_tlb_all(void);
#if 0
void kkm_change_address_space(phys_addr_t pgd_pa);
#endif

void kkm_init_guest_area_redzone(struct kkm_guest_area *ga);
void kkm_verify_guest_area_redzone(struct kkm_guest_area *ga);
bool kkm_verify_bytes(uint8_t *data, uint32_t count, uint8_t value);
void kkm_show_trap_info(struct kkm_guest_area *ga);
void kkm_show_registers(struct kkm_guest_area *ga);
void kkm_show_guest_qwords(struct kkm_guest_area *ga, uint64_t gva,
			   uint64_t count);
void kkm_show_debug_registers(struct kkm_guest_area *ga);

void kkm_copy_xstate_to_kkm_fpu(void *fpregs_state, struct kkm_fpu *kkm_fpu);
void kkm_copy_kkm_fpu_to_xstate(struct kkm_fpu *kkm_fpu, void *fpregs_state);

#endif /* __KKM_MISC_H__ */
