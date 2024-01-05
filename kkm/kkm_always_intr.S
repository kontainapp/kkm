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

.macro kkm_always_intr_entry index
	.align 16
	.global kkm_aie_\index
kkm_aie_\index:
	endbr64
	clac
	pushq	$0
	pushq	$\index
	jmp	kkm_always_intr_common
	int3
	.size kkm_aie_\index, .-kkm_aie_\index
.endm

.macro kkm_always_intr_entry_error index
	.align 16
	.global kkm_aie_\index
kkm_aie_\index:
	endbr64
	clac
	pushq	$\index
	jmp	kkm_always_intr_common_error
	int3
	.size kkm_aie_\index, .-kkm_aie_\index
.endm

.altmacro

	kkm_always_intr_entry %0
	kkm_always_intr_entry %1
	kkm_always_intr_entry %2
	kkm_always_intr_entry %3
	kkm_always_intr_entry %4
	kkm_always_intr_entry %5
	kkm_always_intr_entry %6
	kkm_always_intr_entry %7
	kkm_always_intr_entry_error %8
	kkm_always_intr_entry %9
	kkm_always_intr_entry_error %10
	kkm_always_intr_entry_error %11
	kkm_always_intr_entry_error %12
	kkm_always_intr_entry_error %13
	kkm_always_intr_entry_error %14
	kkm_always_intr_entry %15
	kkm_always_intr_entry %16
	kkm_always_intr_entry_error %17
	kkm_always_intr_entry %18
	kkm_always_intr_entry %19
	kkm_always_intr_entry %20
	kkm_always_intr_entry_error %21

	kkm_always_intr_entry %22
	kkm_always_intr_entry %23
	kkm_always_intr_entry %24
	kkm_always_intr_entry %25
	kkm_always_intr_entry %26
	kkm_always_intr_entry %27
	kkm_always_intr_entry %28
	kkm_always_intr_entry_error %29
	kkm_always_intr_entry_error %30



	.set i, 31
	.rept 225
		kkm_always_intr_entry %i
		.set i, i+1
	.endr

	.align	4096
	.global kkm_always_intr_common
kkm_always_intr_common:
	pushq	%rax
	pushq	%rbx

	testb	$0x3, 0x28(%rsp)
	jz	kkm_always_intr_common_no_swap_gs_1
	swapgs
kkm_always_intr_common_no_swap_gs_1:

	/*
	 * offset is checked in static_assert kkm_main.c 
	 * get current cpu
	 */
	mov	PER_CPU_VAR(pcpu_hot + 12), %eax
	andq	$0x3F, %rax
	shlq	$0x6, %rax
	movabs	$0xffffffffffe06000ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	testq	$0x1, %rbx
	jz	kkm_always_intr_common_native

	/*
	 * kkm intr handler
	 */
	movq	0x10(%rsp), %rax
	shl	$3, %rax
	movabs	$0xffffffffffe05800ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	movq	%rbx, 0x10(%rsp)
	jmp	kkm_always_intr_common_done

kkm_always_intr_common_native:

	/*
	 * native intr handler
	 */
	movq	0x10(%rsp), %rax
	shl	$3, %rax
	movabs	$0xffffffffffe05000ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	movq	%rbx, 0x18(%rsp)

kkm_always_intr_common_done:
	testb	$0x3, 0x28(%rsp)
	jz	kkm_always_intr_common_no_swap_gs_2
	swapgs
kkm_always_intr_common_no_swap_gs_2:

	popq	%rbx
	popq	%rax

	add	$8, %rsp

	retq
	int3

	.size kkm_always_intr_common, .-kkm_always_intr_common

	.align 16
	.global kkm_always_intr_common_error
kkm_always_intr_common_error:
	pushq	%rax
	pushq	%rbx

	testb	$0x3, 0x28(%rsp)
	jz	kkm_always_intr_common_error_no_swap_gs_1
	swapgs
kkm_always_intr_common_error_no_swap_gs_1:


	/*
	 * offset is checked in static_assert kkm_main.c 
	 * get current cpu
	 */
	mov	PER_CPU_VAR(pcpu_hot + 12), %eax
	andq	$0x3F, %rax
	shlq	$0x6, %rax
	movabs	$0xffffffffffe06000ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	testq	$1, %rbx
	jz	kkm_always_intr_common_error_native

	/*
	 * kkm intr handler
	 */
	movq	0x10(%rsp), %rax
	shl	$3, %rax
	movabs	$0xffffffffffe05800ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	movq	%rbx, 0x10(%rsp)
	jmp	kkm_always_intr_common_error_done

kkm_always_intr_common_error_native:

	/*
	 * native intr handler
	 */
	movq	0x10(%rsp), %rax
	shl	$3, %rax
	movabs	$0xffffffffffe05000ULL, %rbx
	addq	%rbx, %rax
	movq	(%rax), %rbx
	movq	%rbx, 0x10(%rsp)

kkm_always_intr_common_error_done:
	testb	$0x3, 0x28(%rsp)
	jz	kkm_always_intr_common_error_no_swap_gs_2
	swapgs
kkm_always_intr_common_error_no_swap_gs_2:

	popq	%rbx
	popq	%rax

	retq
	int3

	.size kkm_always_intr_common_error, .-kkm_always_intr_common_error
