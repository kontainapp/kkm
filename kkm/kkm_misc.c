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

#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/desc.h>
#include <asm/tlbflush.h>
#include <asm/debugreg.h>
#include <asm/cpu_entry_area.h>
#include <asm/fpu/types.h>

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_kontext.h"
#include "kkm_misc.h"
#include "kkm_ioctl.h"

/*
 * dont use
 * this will cause triple exception.
 * as faults cannot be disabled.
 */
void kkm_idt_invalidate(void *address)
{
	struct desc_ptr kkm_idt;

	kkm_idt.size = 0;
	kkm_idt.address = (unsigned long)address;

	load_idt(&kkm_idt);
}

#if 0
/*
 * use CR4 to flush tbl.
 * don't use __native_flush_tlb_global
 * __native_flush_tlb_global is causing return to user space
 */
void kkm_flush_tlb_all(void)
{
	unsigned long cr4;
	unsigned long flags;

	raw_local_irq_save(flags);
	cr4 = this_cpu_read(cpu_tlbstate.cr4);
	// toggle enable global pages
	native_write_cr4(cr4 ^ X86_CR4_PGE);
	// restore original cr4
	native_write_cr4(cr4);
	raw_local_irq_restore(flags);
}

/*
 * kernel uses ASID's and are managed by kernel
 * make sure tlb is completely cleared
 */
void kkm_change_address_space(phys_addr_t pgd_pa)
{
	/* change space */
	write_cr3(pgd_pa);

	/* flush TLB */
	kkm_flush_tlb_all();
}
#endif

/*
 * initialize redzone around guest kernel stack
 */
void kkm_init_guest_area_redzone(struct kkm_guest_area *ga)
{
	memset(ga->redzone_top, REDZONE_DATA, GUEST_STACK_REDZONE_SIZE);
	memset(ga->redzone_bottom, REDZONE_DATA, GUEST_STACK_REDZONE_SIZE);
}

/*
 * verify redzone around guest kernel stack
 */
void kkm_verify_guest_area_redzone(struct kkm_guest_area *ga)
{
#if KKM_REDZONE_CHECK_ENABLE
	if (kkm_verify_bytes(ga->redzone_top, GUEST_STACK_REDZONE_SIZE,
			     REDZONE_DATA) == false) {
		printk(KERN_NOTICE
		       "kkm_verify_guest_area_redzone: top rezone mismatch\n");
	}
	if (kkm_verify_bytes(ga->redzone_bottom, GUEST_STACK_REDZONE_SIZE,
			     REDZONE_DATA) == false) {
		printk(KERN_NOTICE
		       "kkm_verify_guest_area_redzone: bottom rezone mismatch\n");
	}
#endif
}

/*
 * verify buffer for known pattern
 */
bool kkm_verify_bytes(uint8_t *data, uint32_t count, uint8_t value)
{
	int i = 0;
	bool ret_val = true;

	for (i = 0; i < count; i++) {
		if (data[i] == value) {
			continue;
		}
		ret_val = false;
		printk(KERN_NOTICE
		       "kkm_verify_bytes: data mismatch expected(0x%2x) found (0x%2x)\n",
		       REDZONE_DATA, data[i]);
		break;
	}
	return ret_val;
}

void kkm_show_trap_info(struct kkm_guest_area *ga)
{
	printk(KERN_NOTICE
	       "kkm_show_trap_info: thread %lld ga %px cr2 %llx intr_no %llx error %llx rip %llx cs %llx rflags %llx rsp %llx ss %llx\n",
	       ga->kkm_kontext->id, ga, ga->sregs.cr2, ga->intr_no,
	       ga->trap_info.error, ga->trap_info.rip, ga->trap_info.cs,
	       ga->trap_info.rflags, ga->trap_info.rsp, ga->trap_info.ss);
}

void kkm_show_registers(struct kkm_guest_area *ga)
{
	printk(KERN_NOTICE
	       "kkm_show_registers: thread %lld rax %llx rbx %llx rcx %llx rdx %llx\n",
	       ga->kkm_kontext->id, ga->regs.rax, ga->regs.rbx, ga->regs.rcx,
	       ga->regs.rdx);
	printk(KERN_NOTICE
	       "kkm_show_registers: thread %lld rsi %llx rdi %llx rsp %llx rbp %llx\n",
	       ga->kkm_kontext->id, ga->regs.rsi, ga->regs.rdi, ga->regs.rsp,
	       ga->regs.rbp);
	printk(KERN_NOTICE
	       "kkm_show_registers: thread %lld r8 %llx r9 %llx r10 %llx r11 %llx\n",
	       ga->kkm_kontext->id, ga->regs.r8, ga->regs.r9, ga->regs.r10,
	       ga->regs.r11);
	printk(KERN_NOTICE
	       "kkm_show_registers: thread %lld r12 %llx r13 %llx r14 %llx r15 %llx\n",
	       ga->kkm_kontext->id, ga->regs.r12, ga->regs.r13, ga->regs.r14,
	       ga->regs.r15);
	printk(KERN_NOTICE
	       "kkm_show_registers: thread %lld rip %llx rflags %llx\n",
	       ga->kkm_kontext->id, ga->regs.rip, ga->regs.rflags);
}

void kkm_show_guest_qwords(struct kkm_guest_area *ga, uint64_t gva,
			   uint64_t count)
{
	uint64_t byte_count = count * sizeof(uint64_t);
	uint64_t mva;
	uint64_t *buffer_addr = NULL;
	uint64_t i = 0;

	if (kkm_guest_va_to_monitor_va(ga->kkm_kontext, gva, &mva, NULL) ==
	    false) {
		printk(KERN_NOTICE "kkm_show_guest_data: gva to mva failed\n");
		goto error;
	}
	if ((buffer_addr = kzalloc(byte_count, GFP_KERNEL)) == NULL) {
		printk(KERN_NOTICE
		       "kkm_show_guest_data: memory allocation failed szie %lld\n",
		       byte_count);
		goto error;
	}
	if (copy_from_user(buffer_addr, (void *)mva, byte_count)) {
		printk(KERN_NOTICE
		       "kkm_show_guest_data: copy from user failed\n");
		goto error;
	}

	for (i = 0; i < count; i++) {
		printk(KERN_NOTICE "addr %llx data %llx\n", mva + i * 8,
		       buffer_addr[i]);
	}
error:
	if (buffer_addr != NULL) {
		kfree(buffer_addr);
	}
	return;
}

void kkm_show_debug_registers(struct kkm_guest_area *ga)
{
	printk(KERN_NOTICE "kkm_show_debug_registers: control %x\n",
	       ga->debug.control);
	printk(KERN_NOTICE
	       "kkm_show_debug_registers: dr0 %llx dr1 %llx dr2 %llx dr3 %llx\n",
	       ga->debug.registers[0], ga->debug.registers[1],
	       ga->debug.registers[2], ga->debug.registers[3]);
	printk(KERN_NOTICE
	       "kkm_show_debug_registers: dr4 %llx dr5 %llx dr6 %llx dr7 %llx\n",
	       ga->debug.registers[4], ga->debug.registers[5],
	       ga->debug.registers[6], ga->debug.registers[7]);
}

/*
 * convert xstate to kvm_fpu
 */
void kkm_copy_xstate_to_kkm_fpu(void *fpregs_state, struct kkm_fpu *kvm_fpu)
{
	struct xregs_state *xs = (struct xregs_state *)fpregs_state;

	memcpy(kvm_fpu->fpr, xs->i387.st_space, sizeof(xs->i387.st_space));
	kvm_fpu->fcw = xs->i387.cwd;
	kvm_fpu->fsw = xs->i387.swd;
	kvm_fpu->ftwx = (uint8_t)xs->i387.twd;
	kvm_fpu->pad1 = 0;
	kvm_fpu->last_opcode = xs->i387.fop;
	kvm_fpu->last_ip = xs->i387.rip;
	kvm_fpu->last_dp = xs->i387.rdp;
	memcpy(kvm_fpu->xmm, xs->i387.xmm_space, sizeof(xs->i387.xmm_space));
	kvm_fpu->mxcsr = xs->i387.mxcsr;
	kvm_fpu->pad2 = 0;
}

/*
 * convert kvm_fpu to xstate
 */
void kkm_copy_kkm_fpu_to_xstate(struct kkm_fpu *kvm_fpu, void *fpregs_state)
{
	struct xregs_state *xs = (struct xregs_state *)fpregs_state;

	memcpy(xs->i387.st_space, kvm_fpu->fpr, sizeof(xs->i387.st_space));
	xs->i387.cwd = kvm_fpu->fcw;
	xs->i387.swd = kvm_fpu->fsw;
	xs->i387.twd = kvm_fpu->ftwx;
	xs->i387.fop = kvm_fpu->last_opcode;
	xs->i387.rip = kvm_fpu->last_ip;
	xs->i387.rdp = kvm_fpu->last_dp;
	memcpy(xs->i387.xmm_space, kvm_fpu->xmm, sizeof(xs->i387.xmm_space));
	xs->i387.mxcsr = kvm_fpu->mxcsr;
}
