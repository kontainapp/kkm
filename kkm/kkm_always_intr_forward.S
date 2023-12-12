// SPDX-License-Identifier: GPL-2.0
/*
 * Kontain Kernel Module
 *
 * This module enables Kontain unikernel in absence of
 * hardware support for virtualization
 *
 * Copyright (C) 2023-2023 Kontain Inc.
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

/* called for interrupts with error code */
/*	%rdi - target jump address
 *	%rsi - error code
 * 	%rdx - cs
 *	%rcx - ss
 */
	.global kkm_aife_error
kkm_aife_error:
        endbr64
	pushq	%rax
	pushfq
	movq	(%rsp), %rax
	addq	$0x8, %rsp

        testq	$0xf, %rsp
        jz	kkm_aife_error_aligned_sp

        pushq	%rsp	// align stack pointer

	subq	$0x30, %rsp
	movq	%rcx, 0x28(%rsp)
	movq	%rsp, 0x20(%rsp)
	movq	%rax, 0x18(%rsp)
	movq	%rdx, 0x10(%rsp)
	movq	%rsi, 0x8(%rsp)
	movq	kkm_aife_error_ret_unaligned_sp, %rax
	movq	%rax, (%rsp)
	jmpq	*%rdi
kkm_aife_error_ret_unaligned_sp:
        popq	%rsp
	jmp	kkm_aife_ret

kkm_aife_error_aligned_sp:
	subq	$0x30, %rsp
	movq	%rcx, 0x28(%rsp)
	movq	%rsp, 0x20(%rsp)
	movq	%rax, 0x18(%rsp)
	movq	%rdx, 0x10(%rsp)
	movq	%rsi, 0x8(%rsp)
	movq	kkm_aife_error_ret, %rax
	movq	%rax, (%rsp)
	jmpq	*%rdi
kkm_aife_error_ret:
	addq	$0x30, %rsp

	popq	%rax
        retq
        int3
	.size kkm_aife_error, .-kkm_aife_error


/* called for interrupts with no error code */
/*	%rdi - target jump address
 *	%rsi - unused(error code)
 * 	%rdx - cs
 *	%rcx - ss
 */
        .align 16
        .global kkm_aife
kkm_aife:
        endbr64
	pushq	%rax
	pushfq
	movq	(%rsp), %rax
	addq	$0x8, %rsp

        testq	$0xf, %rsp
        jz	kkm_aife_aligned_sp

        pushq	%rsp	// align stack pointer

	subq	$0x28, %rsp
	movq	%rcx, 0x20(%rsp)
	movq	%rsp, 0x18(%rsp)
	movq	%rax, 0x10(%rsp)
	movq	%rdx, 0x8(%rsp)
	movq	kkm_aife_ret_unaligned_sp, %rax
	movq	%rax, (%rsp)
	jmpq	*%rdi
kkm_aife_ret_unaligned_sp:
        popq	%rsp
	jmp	kkm_aife_ret

kkm_aife_aligned_sp:
	subq	$0x28, %rsp
	movq	%rcx, 0x20(%rsp)
	movq	%rsp, 0x18(%rsp)
	movq	%rax, 0x10(%rsp)
	movq	%rdx, 0x8(%rsp)
	movq	kkm_aife_ret, %rax
	movq	%rax, (%rsp)
	jmpq	*%rdi
kkm_aife_ret:
	addq	$0x28, %rsp

	popq	%rax
        retq
        int3
        .size kkm_aife, .-kkm_aife
