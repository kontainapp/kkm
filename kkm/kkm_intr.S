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
	 * set flags we cant depend on guest
	 */
	clac
	clc

	/*
	 * change kernel thread local storage
	 */
	swapgs

	/*
	 * create temporary register
	 */
	pushq	%rdi

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
	 * load kx area to temporary register
	 */

	# save registers
	iretq
	.size kkm_intr_entry_asm, .-kkm_intr_entry_asm

	.global kkm_intr_fill
kkm_intr_fill:
	.fill  TOTAL_CODE_SIZE - kkm_intr_fill - kkm_intr_start, 1, 0x90
	.size kkm_intr_fill, .-kkm_intr_fill
