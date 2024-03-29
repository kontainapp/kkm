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

#include <linux/linkage.h>
#include <asm/percpu.h>
#include <asm/asm-offsets.h>
#include "kkm_offsets.h"

	.text
	.align 4096
/*
 * %rdi -- guest private area
 * %rsi -- stack start
 *
 * get a clean break in CPU context here
 * C-ABI allows us to discard all other registers
 * only calee saved registers are pushed to stack
 * we are on native kernel stack on entry and
 * guest kernel stack before calling C functions
 */
	.align 16
	.global kkm_switch_to_gk_asm
	.type kkm_switch_to_gk_asm, @function
kkm_switch_to_gk_asm:
	.cfi_startproc

	/*
	 * save callee saved registers according to C-ABI
	 */
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	/*
	 * save native kernel stack top in guest private area
	 * use stack to to restore to native kernel
	 */
	movq	%rsp, OFF_NK_STACK(%rdi)

	/*
	 * switch stacks
	 *	old - native kernel stack
	 *	new - guest kernel stack
	 */
	movq	%rsi, %rsp

	/* guest private area is still in %rdi */
	call kkm_guest_kernel_start_payload

	/* NOTREACHED */

kkm_restore_label:
	/*
	 * following code is for return path
	 */
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp
	retq
	int3

	.cfi_endproc
	.size kkm_switch_to_gk_asm, .-kkm_switch_to_gk_asm

/*
 * %rdi -- native kernel cr3
 * %rsi -- saved stack
 *
 * on entry we are using guest kernel stack
 * we are on native kernel stack before returning to kernel functions
 */
	.align 16
	.global kkm_switch_to_hk_asm
	.type kkm_switch_to_hk_asm, @function
kkm_switch_to_hk_asm:
	.cfi_startproc

	/*
	 * switch address space
	 *	old - guest kernel address space
	 *	new - native kernel address space
	 */
	movq	%rdi, %cr3

	andq	$0xFF, %rdi
	cmpq	$0, %rdi
	jne	skip_tlb_flush_1

	/*
	 * invalidate tlb
	 * toggle CR4_PGE bit and then restore
	 */
	movq	%cr4, %rax
	movq	%rax, %rbx
	xorq	$0x80, %rax
	movq	%rax, %cr4
	movq	%rbx, %cr4

skip_tlb_flush_1:
	/*
	 * switch stacks
	 *	old - guest kernel stack
	 *	new - native kernel stack
	 */
	movq	%rsi, %rsp
	jmp	kkm_restore_label

	retq
	int3
	/* NOTREACHED */

	.cfi_endproc
	.size kkm_switch_to_hk_asm, .-kkm_switch_to_hk_asm

/*
 * %rdi -- guest private area
 *
 * register context is set by monitor using KKM_SET_REGS and KKM_SET_SREGS
 * in guest private area. move them from guest private area to registers.
 *
 * on entry we are using guest kernel stack
 * in the middle switch to iret stack
 * by the end of this function we are running on guest payload stack
 */
	.align 16
	.global kkm_switch_to_gp_asm
	.type kkm_switch_to_gp_asm, @function
kkm_switch_to_gp_asm:
	.cfi_startproc

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

	/*
	 * we need some temporary registers to
	 * setup iret stack. save them on the stack
	 */
	movq	OFF_RSP(%rdi), %rsi	# move %rsp from guest_private_area to scratch(%rsi)
	pushq	%rsi			# save %rsp=scratch(%rsi) on stack
	movq	OFF_RDI(%rdi), %rsi	# move %rdi from guest_private_area to scratch(%rsi)
	pushq	%rsi			# save %rdi=scratch(%rsi) on stack
	movq	OFF_RSI(%rdi), %rsi

	movq	OFF_RDX(%rdi), %rdx
	movq	OFF_RCX(%rdi), %rcx
	movq	OFF_RBX(%rdi), %rbx
	movq	OFF_RAX(%rdi), %rax


	/*
	 * save current stack pointer
	 * used for testing
	 */
	movq	%rsp, %rbx

	/*
	 * load ga->payload_entry_stack
	 * this is our iret stack. this needs to be 16 byte aligned
	 */
	movq	%rdi, %rax
	addq	$OFF_PAY_ENT_STK, %rax

	/*
	 * switch stacks
	 *	old - guest kernel stack
	 *	new - iret stack
	 */
	movq	%rax, %rsp

	/*
	 * setup iret stack
	 *	----------- bottom of stack
	 *	|   SS    |
	 *	-----------
	 *	|   RSP   |
	 *	-----------
	 *	|  RFLAGS |
	 *	-----------
	 *	|   CS    |
	 *	-----------
	 *	|   RIP   |
	 *	----------- top of stack
	 */
	pushq	OFF_GP_SS(%rdi)
	pushq	OFF_RSP(%rdi)
	pushq	OFF_RFLAGS(%rdi)
	pushq	OFF_GP_CS(%rdi)
	pushq	OFF_RIP(%rdi)

	/*
	 * put scratch registers on iret stack
	 */
	pushq	OFF_RDI(%rdi)	# user %rdi
	pushq	OFF_RAX(%rdi)	# user %rax
	pushq	OFF_RBX(%rdi)	# user %rbx

	movq	$0, %rax
	movq	%rax, %cr2

	/*
	 * switch address space
	 *	old - guest kernel address space
	 *	new - guest payload address space
	 */
	movq	OFF_GP_CR3(%rdi), %rax
	movq	%rax, %cr3

	andq	$0xFF, %rax
	cmpq	$0, %rax
	jne	skip_tlb_flush_2

	/*
	 * invalidate tlb
	 * toggle CR4_PGE bit and then restore
	 */
	movq	%cr4, %rax
	movq	%rax, %rbx
	xorq	$0x80, %rax
	movq	%rax, %cr4
	movq	%rbx, %cr4

skip_tlb_flush_2:
	/*
	 * rbx, rax, rdi were used as scratch registers.
	 * pop to set correct conext
	 */
	popq	%rbx
	popq	%rax
	popq	%rdi

	/*
	 * set kernel thread local storage to NULL
	 */
	swapgs

	/*
	 * start payload with context picked up from stack
	 */
	iretq

	.cfi_endproc
	.size kkm_switch_to_gp_asm, .-kkm_switch_to_gp_asm

/*
 * keep at the end of this file
 */
	.align 16
	.global kkm_guest_entry_end
	.type kkm_guest_entry_end, @function
kkm_guest_entry_end:
	.cfi_startproc

	retq
	int3

	.cfi_endproc
	.size kkm_guest_entry_end, .-kkm_guest_entry_end

