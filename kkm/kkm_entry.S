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

.code64
.section .entry.text, "ax"

/*
 * %rdi -- guest private area
 * %rsi -- pointer to kkm
 * %rdx -- stack start
 */
.global kkm_switch_to_guest
.align 16
kkm_switch_to_guest:
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	/* save kkm */
	movq	%rsi, (%rdi)
	/* save guest private pointer */
	movq	%rdi, 8(%rdi)
	/* save kernel stack */
	movq	%rsp, 16(%rdi)

	mov	%rdx, %rsp

	/* guest private area is still in %rdi */
	call kkm_guest_kernel_start

	/* following code never executes */
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	.type kkm_switch_to_guest, @function
.size kkm_switch_to_guest, .-kkm_switch_to_guest

.global kkm_switch_to_host
.align 16
kkm_switch_to_host:

	movq	16(%rdi), %rsi
	mov	%rsi, %rsp

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	.type kkm_switch_to_host, @function
.size kkm_switch_to_host, .-kkm_switch_to_host

.globl kkm_trap_entry
.align 16
kkm_trap_entry:
.size kkm_trap_entry, .-kkm_trap_entry
