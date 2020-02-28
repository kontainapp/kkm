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

#define OFF_KONTEXT (8 * 0) /* struct kkm_kontext * */
#define OFF_GAB (8 * 1) /* guest_area_beg */
#define OFF_NK_STACK (8 * 2) /* native_kernel_stack */
#define OFF_GK_CR3 (8 * 3) /* guest_kernel_cr3 */
#define OFF_GK_CR4 (8 * 4) /* guest_kernel_cr3 */
#define OFF_GP_CR3 (8 * 5) /* guest_payload_cr3 */
#define OFF_GSV (8 * 6) /* guest_stack_variable_address */
#define OFF_GP_CS (8 * 7) /* guest_payload_cs */
#define OFF_GP_SS (8 * 8) /* guest_payload_ss */

#define	REG_BASE_OFF	(128)

#define OFF_RAX ((REG_BASE_OFF) + 8 * 0) /* regs.rax */
#define OFF_RBX ((REG_BASE_OFF) + 8 * 1) /* regs.rbx */
#define OFF_RCX ((REG_BASE_OFF) + 8 * 2) /* regs.rcx */
#define OFF_RDX ((REG_BASE_OFF) + 8 * 3) /* regs.rdx */

#define OFF_RSI ((REG_BASE_OFF) + 8 * 4) /* regs.rsi */
#define OFF_RDI ((REG_BASE_OFF) + 8 * 5) /* regs.rdi */
#define OFF_RSP ((REG_BASE_OFF) + 8 * 6) /* regs.rsp */
#define OFF_RBP ((REG_BASE_OFF) + 8 * 7) /* regs.rbp */

#define OFF_R8 ((REG_BASE_OFF) + 8 * 8) /* regs.rax */
#define OFF_R9 ((REG_BASE_OFF) + 8 * 9) /* regs.rbx */
#define OFF_R10 ((REG_BASE_OFF) + 8 * 10) /* regs.rcx */
#define OFF_R11 ((REG_BASE_OFF) + 8 * 11) /* regs.rdx */

#define OFF_R12 ((REG_BASE_OFF) + 8 * 12) /* regs.rsi */
#define OFF_R13 ((REG_BASE_OFF) + 8 * 13) /* regs.rdi */
#define OFF_R14 ((REG_BASE_OFF) + 8 * 14) /* regs.rsp */
#define OFF_R15 ((REG_BASE_OFF) + 8 * 15) /* regs.rbp */

#define OFF_RIP ((REG_BASE_OFF) + 8 * 16) /* regs.rip */
#define OFF_RFLAGS ((REG_BASE_OFF) + 8 * 17) /* regs.rflags */

#define OFF_PAY_ENT_STK	(4096)	/* payload_entry_stack bottom */

#endif /* __KKM_OFFSETS_H__ */
