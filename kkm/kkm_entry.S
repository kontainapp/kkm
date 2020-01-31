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

#include <linux/linkage.h>
#include "kkm_offsets.h"

.code64
.section .entry.text, "ax"

/*
 * %rdi -- guest private area
 * %rsi -- pointer to kkm
 * %rdx -- stack start
 */
.global kkm_switch_to_gk_asm
.align 16
kkm_switch_to_gk_asm:
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	/* save kkm */
	movq	%rsi, OFF_KONTEXT(%rdi)
	/* save guest private pointer */
	movq	%rdi, OFF_GAB(%rdi)
	/* save native kernel stack */
	movq	%rsp, OFF_NK_STACK(%rdi)

	mov	%rdx, %rsp

	/* guest private area is still in %rdi */
	call kkm_guest_kernel_start_payload

	/* following code never executes */
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	.type kkm_switch_to_gk_asm, @function
.size kkm_switch_to_gk_asm, .-kkm_switch_to_gk_asm

/*
 * %rdi -- guest private area
 */
.global kkm_switch_to_hk_asm
.align 16
kkm_switch_to_hk_asm:

	/* restore native kernel stack */
	movq	OFF_NK_STACK(%rdi), %rsi
	mov	%rsi, %rsp

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	.type kkm_switch_to_hk_asm, @function
.size kkm_switch_to_hk_asm, .-kkm_switch_to_hk_asm

/*
 * %rdi -- guest private area
 */
.global kkm_switch_to_gp_asm
.align 16
kkm_switch_to_gp_asm:
	movq OFF_R15(%rdi), %r15
	movq OFF_R14(%rdi), %r14
	movq OFF_R13(%rdi), %r13
	movq OFF_R12(%rdi), %r12

	movq OFF_R11(%rdi), %r11
	movq OFF_R10(%rdi), %r10
	movq OFF_R9(%rdi), %r9
	movq OFF_R8(%rdi), %r8

	movq OFF_RBP(%rdi), %rbp
	movq OFF_RSP(%rdi), %rsi
	pushq %rsi
	movq OFF_RDI(%rdi), %rsi
	pushq %rsi
	movq OFF_RSI(%rdi), %rsi

	movq OFF_RDX(%rdi), %rdx
	movq OFF_RCX(%rdi), %rcx
	movq OFF_RBX(%rdi), %rbx
	movq OFF_RAX(%rdi), %rax

	popq %rdi
	popq %rsp

	swapgs
	sysretq
.size kkm_switch_to_gp_asm, .-kkm_switch_to_gp_asm

.global kkm_trap_entry
.align 16
kkm_trap_entry:
	iretq
.size kkm_trap_entry, .-kkm_trap_entry
