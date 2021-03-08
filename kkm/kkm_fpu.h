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

#ifndef __KKM_FPU_H__
#define __KKM_FPU_H__

void kkm_fpu_save_xstate_xsaves(void *xstate_buf);
void kkm_fpu_restore_xstate_xsaves(void *xstate_buf);
void kkm_fpu_save_xstate_xsave(void *xstate_buf);
void kkm_fpu_restore_xstate_xsave(void *xstate_buf);

#endif /* __KKM_FPU_H__ */
