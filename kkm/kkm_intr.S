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


#define	FIRST_IRQ	(0)
#define	TOTAL_IRQ	(256)
#define TOTAL_CODE_SIZE (2 * 4096)

/*
 * This code will be physically copied to 
 * kx entry area which starts right next to IDT
 * This code is running on stack from tss->stack0.
 */
	.text
	.align 4096
	.global kkm_intr_start
kkm_intr_start:
	irqnumber=0
	.rept (TOTAL_IRQ-FIRST_IRQ)
	pushq	irqnumber
	jmp	kkm_intr_entry_asm
	.align 8
	irqnumber=irqnumber+1
	.endr
	.size kkm_intr_start, .-kkm_intr_start

	.global kkm_intr_entry_asm
kkm_intr_entry_asm:
	/*
	 * set flags, entered from unknown code
	 */
	clac
	clc

	/*
	 * change to kernel thread local storage
	 */
	swapgs

	/*
	 * store dummy error code to stack
	 */
	pushq	$-1

	/*
	 * create scratch registers
	 */
	pushq	%rdi
	pushq	%rsi
	/*
	 * trap stack looks like this now
	 *      ----------- bottom of stack
	 *      |   SS    |
	 *      ----------
	 *      |   RSP   |
	 *      -----------
	 *      |  RFLAGS |
	 *      -----------
	 *      |   CS    |
	 *      -----------
	 *      |   RIP   |
	 *      -----------
	 *      |  ERROR  |
	 *      -----------
	 *      | INTR NO |
	 *      -----------
	 *      |   RDI   |
	 *      -----------
	 *      |   RSI   |
	 *      ----------- top of stack
	 */

	/*
	 * save current stack top to rsi
	 * we need this to save trap information
	 */
	movq	%rsp, %rsi

	/*
	 * switch to guest kernel address space
	 * kernel and payload PML4 are allocated next to each
	 * other(even page is kernel, odd page is guest) in physical memory.
	 * no need to flush TLB
	 * guest kernel address space is superset of guest payload address space
	 */
	movq	%cr3, %rdi
	andq	$0xffffffffffffefff, %rdi
	mov	%rdi, %cr3

	/*
	 * %rsp is currently in guest area
	 * masking off bottom 12 bits points to ga in kx area
	 */
	movq	%rsp, %rdi
	andq	$0xffffffffffffe000, %rdi

	/*
	 * switch stacks
	 *    old - intr context stack
	 *    new - guest kernel stack
	 */
	movq	%rdi, %rsp
	addq	OFF_GUEST_STK, %rsp

	/*
	 * save registers to kkm_guest_area
	 */
	movq	%r15, OFF_R15(%rdi)
	movq	%r14, OFF_R14(%rdi)
	movq	%r13, OFF_R13(%rdi)
	movq	%r12, OFF_R12(%rdi)

	movq	%r11, OFF_R11(%rdi)
	movq	%r10, OFF_R10(%rdi)
	movq	%r9, OFF_R9(%rdi)
	movq	%r8, OFF_R8(%rdi)

	movq	%rbp, OFF_RBP(%rdi)
	/*
	 * guest %rsi, %rsp, %rdi are on interrupt stack
	 * save them using %rsi which has interrupt stack
	 * use %rbp as scratch register
	 */
	movq	OFF_TS_RSP(%rsi), %rbp
	movq	%rbp, OFF_RSP(%rdi)

	movq	OFF_TS_RDI(%rsi), %rbp
	movq	%rbp, OFF_RDI(%rdi)

	movq	OFF_TS_RSI(%rsi), %rbp
	movq	%rbp, OFF_RSI(%rdi)

	movq	%rdx, OFF_RDX(%rdi)
	movq	%rcx, OFF_RCX(%rdi)
	movq	%rbx, OFF_RBX(%rdi)
	movq	%rax, OFF_RAX(%rdi)

	/*
	 * save trap information from trap stack to kkm_guest_area
	 */
	movq	OFF_TS_ERROR(%rsi), %rbp
	movq	%rbp, OFF_TI_ERROR(%rdi)

	movq	OFF_TS_RIP(%rsi), %rbp
	movq	%rbp, OFF_TI_RIP(%rdi)

	movq	OFF_TS_CS(%rsi), %rbp
	movq	%rbp, OFF_TI_CS(%rdi)

	movq	OFF_TS_RFLAGS(%rsi), %rbp
	movq	%rbp, OFF_TI_RFLAGS(%rdi)

	movq	OFF_TS_RSP(%rsi), %rbp
	movq	%rbp, OFF_TI_RSP(%rdi)

	movq	OFF_TS_SS(%rsi), %rbp
	movq	%rbp, OFF_TI_SS(%rdi)

	/*
	 * save interrupt no
	 */
	movq	OFF_TS_INTR_NO(%rsi), %rbp
	movq	%rbp, OFF_TI_INTR_NO(%rdi)

	call	kkm_switch_to_host_kernel

	/* NOTREACHED */
	iretq
	.size kkm_intr_entry_asm, .-kkm_intr_entry_asm

	.global kkm_intr_fill
kkm_intr_fill:
	.fill  TOTAL_CODE_SIZE - kkm_intr_fill - kkm_intr_start, 1, 0x90
	.size kkm_intr_fill, .-kkm_intr_fill
