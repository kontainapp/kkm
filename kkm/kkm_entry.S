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
#include <asm/percpu.h>
#include <asm/asm-offsets.h>
#include "kkm_offsets.h"

	.text
/*
 * %rdi -- guest private area
 * %rsi -- pointer to kkm
 * %rdx -- stack start
 */
ENTRY(kkm_switch_to_gk_asm)

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

END(kkm_switch_to_gk_asm)

/*
 * %rdi -- guest private area
 */
ENTRY(kkm_switch_to_hk_asm)

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

END(kkm_switch_to_hk_asm)

/*
 * %rdi -- guest private area
 */
ENTRY(kkm_switch_to_gp_asm)

	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	movq	OFF_R15(%rdi), %r15
	movq	OFF_R14(%rdi), %r14
	movq	OFF_R13(%rdi), %r13
	movq	OFF_R12(%rdi), %r12

	movq	OFF_R11(%rdi), %r11
	movq	OFF_R10(%rdi), %r10
	movq	OFF_R9(%rdi), %r9
	movq	OFF_R8(%rdi), %r8

	movq	OFF_RBP(%rdi), %rbp
	# use %rsi as scratch register. It is not yet loaded
	movq	OFF_RSP(%rdi), %rsi	# move %rsp from guest_private_area to scratch(%rsi)
	pushq	%rsi			# save %rsp=scratch(%rsi) on stack
	movq	OFF_RDI(%rdi), %rsi	# move %rdi from guest_private_area to scratch(%rsi)
	pushq	%rsi			# save %rdi=scratch(%rsi) on stack
	movq	OFF_RSI(%rdi), %rsi

	movq	OFF_RDX(%rdi), %rdx
	movq	OFF_RCX(%rdi), %rcx
	movq	OFF_RBX(%rdi), %rbx
	movq	OFF_RAX(%rdi), %rax


	movq	%rsp, %rbx		# save guest kernel stack in %rbx

	# using %rdi->payload_entry_stack as trampoline
	movq	%rdi, %rax
	addq	$OFF_PAY_ENT_STK, %rax
	movq	%rax, %rsp

	pushq	OFF_GP_SS(%rdi)
	pushq	OFF_RSP(%rdi)
	pushq	OFF_RFLAGS(%rdi)
	pushq	OFF_GP_CS(%rdi)
	pushq	OFF_RIP(%rdi)

	pushq	OFF_RDI(%rdi)	# user %rdi
	pushq	OFF_RAX(%rdi)	# user %rax
	pushq	OFF_RBX(%rdi)	# user %rbx

	movq	OFF_GP_CR3(%rdi), %rax
	movq	%rax, %cr3

#if 1
	popq	%rbx
	popq	%rax
	popq	%rdi

	swapgs
	iretq
#else
	movq	%rbx, %rsp

	popq	%rdi
	popq	%rax

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp

	retq
#endif

END(kkm_switch_to_gp_asm)

ENTRY(kkm_trap_entry_asm)

#swapgs
	pushq	%rdi			# save %rdi on stack

	movq	%rsp, %rdi
	andq	$0xffffffffffffe000, %rdi	# begining of current stack is register save area

	movq	%r15, OFF_R15(%rdi)
	movq	%r14, OFF_R14(%rdi)
	movq	%r13, OFF_R13(%rdi)
	movq	%r12, OFF_R12(%rdi)

	movq	%r11, OFF_R11(%rdi)
	movq	%r10, OFF_R10(%rdi)
	movq	%r9, OFF_R9(%rdi)
	movq	%r8, OFF_R8(%rdi)

	movq	%rbp, OFF_RBP(%rdi)
	movq	%rsp, OFF_RSP(%rdi)
	# rbp is already saved use %rbp as scratch
	movq	(%rsp), %rbp		# move %rdi from stack to scratch reg
	movq	%rbp, OFF_RDI(%rdi)	# move scratch(%rdi) to register save area
	movq	%rsi, OFF_RSI(%rdi)

	movq	%rdx, OFF_RDX(%rdi)
	movq	%rcx, OFF_RCX(%rdi)
	movq	%rbx, OFF_RBX(%rdi)
	movq	%rax, OFF_RAX(%rdi)

	popq	%rdi

#if 0
	iretq
#else
	call	kkm_switch_to_host_kernel
	retq
#endif

END(kkm_trap_entry_asm)
