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

#ifndef __KKM_IDT_H__
#define __KKM_IDT_H__

int kkm_idt_init(void);
void kkm_idt_cleanup(void);
int kkm_idt_get_desc(struct desc_ptr *native_desc, struct desc_ptr *guest_desc);
void kkm_idt_set_id(int cpu, uint64_t id);
uint64_t kkm_idt_get_id(int cpu);

#endif /* __KKM_IDT_H__ */
