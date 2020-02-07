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

#ifndef __KKM_OFFSETS_H__
#define __KKM_OFFSETS_H__

/*
 * offsets of variables into guest area
 */

#define OFF_KONTEXT (0) /* struct kkm_kontext * */
#define OFF_GAB (8) /* guest_area_beg */
#define OFF_NK_STACK (16) /* native_kernel_stack */
#define OFF_GK_CR3 (24) /* guest_kernel_cr3 */
#define OFF_GP_CR3 (32) /* guest_payload_cr3 */
#define OFF_GSV (40) /* guest_stack_variable_address */
#define OFF_GP_CS (48) /* guest_payload_cs */
#define OFF_GP_SS (56) /* guest_payload_ss */

#define OFF_RAX (64) /* regs.rax */
#define OFF_RBX (72) /* regs.rbx */
#define OFF_RCX (80) /* regs.rcx */
#define OFF_RDX (88) /* regs.rdx */

#define OFF_RSI (96) /* regs.rsi */
#define OFF_RDI (104) /* regs.rdi */
#define OFF_RSP (112) /* regs.rsp */
#define OFF_RBP (120) /* regs.rbp */

#define OFF_R8 (128) /* regs.rax */
#define OFF_R9 (136) /* regs.rbx */
#define OFF_R10 (144) /* regs.rcx */
#define OFF_R11 (152) /* regs.rdx */

#define OFF_R12 (160) /* regs.rsi */
#define OFF_R13 (168) /* regs.rdi */
#define OFF_R14 (176) /* regs.rsp */
#define OFF_R15 (184) /* regs.rbp */

#define OFF_RIP (192) /* regs.rip */
#define OFF_RFLAGS (200) /* regs.rflags */

#endif /* __KKM_OFFSETS_H__ */
