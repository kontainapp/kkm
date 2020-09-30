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
