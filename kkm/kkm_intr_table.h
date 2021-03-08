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

#ifndef __KKM_INTR_TABLE_H__
#define __KKM_INTR_TABLE_H__

extern uint64_t intr_function_pointers[NR_VECTORS];
extern uint64_t intr_forward_pointers[NR_VECTORS];

#endif /* __KKM_INTR_TABLE_H__ */
