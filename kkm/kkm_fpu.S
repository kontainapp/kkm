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
 * %rdi -- xsave area
 * save current processor state to xsave area
 */
	.align 16
	.global kkm_fpu_save_xstate_xsaves
	.type kkm_fpu_save_xstate_xsaves, @function
kkm_fpu_save_xstate_xsaves:
	.cfi_startproc

	pushq	%rax
	pushq	%rdx

	movq	$-1, %rax
	movq	%rax, %rdx
	xsaves64	(%rdi)

	popq	%rdx
	popq	%rax

	retq

	.cfi_endproc
	.size kkm_fpu_save_xstate_xsaves, .-kkm_fpu_save_xstate_xsaves

/*
 * %rdi -- xsave area
 * restore current processor state from xsave area
 */
	.align 16
	.global kkm_fpu_restore_xstate_xsaves
	.type kkm_fpu_restore_xstate_xsaves, @function
kkm_fpu_restore_xstate_xsaves:
	.cfi_startproc

	pushq	%rax
	pushq	%rdx

	movq	$-1, %rax
	movq	%rax, %rdx

	xrstors64	(%rdi)

	popq	%rdx
	popq	%rax

	retq

	.cfi_endproc
	.size kkm_fpu_restore_xstate_xsaves, .-kkm_fpu_restore_xstate_xsaves


/*
 * %rdi -- xsave area
 * save current processor state to xsave area
 */
	.align 16
	.global kkm_fpu_save_xstate_xsave
	.type kkm_fpu_save_xstate_xsave, @function
kkm_fpu_save_xstate_xsave:
	.cfi_startproc

	pushq	%rax
	pushq	%rdx

	movq	$-1, %rax
	movq	%rax, %rdx
	xsave64	(%rdi)

	popq	%rdx
	popq	%rax

	retq

	.cfi_endproc
	.size kkm_fpu_save_xstate_xsave, .-kkm_fpu_save_xstate_xsave

/*
 * %rdi -- xsave area
 * restore current processor state from xsave area
 */
	.align 16
	.global kkm_fpu_restore_xstate_xsave
	.type kkm_fpu_restore_xstate_xsave, @function
kkm_fpu_restore_xstate_xsave:
	.cfi_startproc

	pushq	%rax
	pushq	%rdx

	movq	$-1, %rax
	movq	%rax, %rdx

	xrstor64	(%rdi)

	popq	%rdx
	popq	%rax

	retq

	.cfi_endproc
	.size kkm_fpu_restore_xstate_xsave, .-kkm_fpu_restore_xstate_xsave
