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

#include <linux/mm.h>
#include <asm/tlbflush.h>
#include <asm/debugreg.h>
#include <asm/cpu_entry_area.h>

#include "kkm.h"
#include "kkm_kontext.h"
#include "kkm_mm.h"
#include "kkm_entry.h"

DEFINE_PER_CPU(struct kkm_kontext *, current_kontext);

void kkm_hw_debug_registers_save(uint64_t *registers);
void kkm_hw_debug_registers_restore(uint64_t *registers);

int kkm_kontext_init(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	// stack0
	ret_val = kkm_mm_allocate_page(&kkm_kontext->guest_area_page,
				       &kkm_kontext->guest_area, NULL);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontext_init: Failed to allocate memory for stack0 error(%d)\n",
		       ret_val);
		goto error;
	}

	printk(KERN_NOTICE "kkm_kontext_init: stack0 page %lx va %p\n",
	       (unsigned long)kkm_kontext->guest_area_page,
	       kkm_kontext->guest_area);

error:
	if (ret_val != 0) {
		kkm_kontext_cleanup(kkm_kontext);
	}
	return ret_val;
}

void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext)
{
	if (kkm_kontext->guest_area_page != NULL) {
		free_page((unsigned long long)kkm_kontext->guest_area);
		kkm_kontext->guest_area_page = NULL;
		kkm_kontext->guest_area = NULL;
	}
}

// running in native kernel address space
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;
	struct kkm_guest_area *ga =
		(struct kkm_guest_area *)kkm_kontext->guest_area;
	int cpu = -1;
	struct cpu_entry_area *cea = NULL;
#if 1
	// delete
	uint64_t efer = 0;
	uint64_t star = 0;
	uint64_t lstar = 0;

	rdmsrl(MSR_EFER, efer);
	rdmsrl(MSR_STAR, star);
	rdmsrl(MSR_LSTAR, lstar);

	printk(KERN_NOTICE "kkm_kontext_switch_kernel: EFER %llx STAR %llx LSTAR %llx\n",
	       efer, star, lstar);
#endif
	printk(KERN_NOTICE "kkm_kontext_switch_kernel:\n");

	cpu = get_cpu();
	per_cpu(current_kontext, cpu) = kkm_kontext;
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: cpu %d %llx\n", cpu,
	       (unsigned long long)&cpu);

	memset(ga->redzone, 0xa5, GUEST_STACK_REDZONE_SIZE);
	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: before %llx %llx %llx %llx\n",
	       (unsigned long long)ga->kkm_kontext, ga->guest_area_beg,
	       ga->native_kernel_stack, ga->guest_stack_variable_address);

	// save trampoline stack
	cea = get_cpu_entry_area(cpu);
	memcpy(&ga->native_entry_stack, &cea->entry_stack_page.stack, sizeof(struct entry_stack));

	// save native kernel address space
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	kkm_kontext->native_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: native kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->native_kernel_cr3, kkm_kontext->native_kernel_cr4);

	// flush TLB
	__native_flush_tlb_global();

	// change to guest kernel address space
	write_cr3(kkm->guest_kernel_pa);

	kkm_kontext->guest_kernel_cr3 = __read_cr3();
	kkm_kontext->guest_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: guest kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->guest_kernel_cr3, kkm_kontext->guest_kernel_cr4);

	savesegment(ds, kkm_kontext->native_kernel_ds);
	savesegment(es, kkm_kontext->native_kernel_es);

	savesegment(fs, kkm_kontext->native_kernel_fs);
	rdmsrl(MSR_FS_BASE, kkm_kontext->native_kernel_fs_base);
	savesegment(gs, kkm_kontext->native_kernel_gs);
	rdmsrl(MSR_GS_BASE, kkm_kontext->native_kernel_gs_base);
	rdmsrl(MSR_KERNEL_GS_BASE, kkm_kontext->native_kernel_gs_kern_base);

	savesegment(ss, kkm_kontext->native_kernel_ss);

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: segments ds %x es %x fs %x fsbase %lx gs %x gsbase %lx gskernbase %lx ss %x\n",
	       kkm_kontext->native_kernel_ds, kkm_kontext->native_kernel_es,
	       kkm_kontext->native_kernel_fs,
	       kkm_kontext->native_kernel_fs_base,
	       kkm_kontext->native_kernel_gs,
	       kkm_kontext->native_kernel_gs_base,
	       kkm_kontext->native_kernel_gs_kern_base,
	       kkm_kontext->native_kernel_ss);

	ga->guest_payload_cr3 = kkm->guest_payload_pa;

	kkm_hw_debug_registers_save(kkm_kontext->native_debug_registers);

	kkm_switch_to_gk_asm(ga, kkm_kontext, (unsigned long long)ga->redzone);

	kkm_hw_debug_registers_restore(kkm_kontext->native_debug_registers);

#if 0
	// restore is done as part of trap/intr, delete once everything works
	// this code runs in native kernel context

	// flush TLB, and restore native kernel cr4
	__native_flush_tlb_global();

	// restore native kernel address space
	write_cr3(kkm_kontext->native_kernel_cr3);

	// restore native kernel segment registers
	loadsegment(ds, kkm_kontext->native_kernel_ds);
	loadsegment(es, kkm_kontext->native_kernel_es);

	loadsegment(fs, kkm_kontext->native_kernel_fs);
	wrmsrl(MSR_FS_BASE, kkm_kontext->native_kernel_fs_base);

	load_gs_index(kkm_kontext->native_kernel_gs);
	wrmsrl(MSR_GS_BASE, kkm_kontext->native_kernel_gs_base);
	wrmsrl(MSR_KERNEL_GS_BASE, kkm_kontext->native_kernel_gs_kern_base);

	loadsegment(ss, __KERNEL_DS);
#endif

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: after %llx %llx %llx %llx\n",
	       (unsigned long long)ga->kkm_kontext, ga->guest_area_beg,
	       ga->native_kernel_stack, ga->guest_stack_variable_address);

	printk(KERN_NOTICE "kkm_kontext_switch_kernel: ret_val %d %llx\n",
	       ret_val, (unsigned long long)&ret_val);

	return ret_val;
}

// running in guest kernel address space
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga)
{
	int cpu = 0x66;
	struct cpu_entry_area *cea = NULL;

	cpu = get_cpu();
	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: cpu %d %llx\n", cpu,
	       (unsigned long long)&cpu);

	// delete - moved to asm file
	// switch to guest payload address space
	// write_cr3(ga->guest_payload_cr3);

	ga->guest_stack_variable_address = (unsigned long long)&cpu;

	loadsegment(ds, 0);
	loadsegment(es, 0);

	loadsegment(fs, 0);
	wrmsrl(MSR_FS_BASE, ga->sregs.fs.base);

	ga->guest_payload_cs = __USER_CS;
	ga->guest_payload_ss = __USER_DS;

	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: fsbase %llx usercs %llx userss %llx\n",
	       ga->sregs.fs.base, ga->guest_payload_cs, ga->guest_payload_ss);

	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: rip %llx rsp %llx rflags %llx\n",
	       ga->regs.rip, ga->regs.rsp, ga->regs.rflags);

	kkm_hw_debug_registers_restore(ga->debug.registers);

	// flush TLB
	__native_flush_tlb_global();

	kkm_switch_to_gp_asm(ga);

	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: returned from guest call\n");

	cea = get_cpu_entry_area(cpu);
	memcpy(&ga->payload_entry_stack, &cea->entry_stack_page.stack, sizeof(struct entry_stack));

	kkm_trap_entry();
	//kkm_switch_to_host_kernel();
}

// should be called from trap code, with zero context
void kkm_switch_to_host_kernel(void)
{
	int cpu = -1;
	struct kkm_kontext *kkm_kontext = NULL;
	struct kkm_guest_area *ga = NULL;

	printk(KERN_NOTICE "kkm_switch_to_host_kernel:\n");

	cpu = get_cpu();
	kkm_kontext = per_cpu(current_kontext, cpu);
	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;

	printk(KERN_NOTICE "kkm_switch_to_host_kernel: cpu %d %llx\n", cpu,
	       (unsigned long long)&cpu);

	kkm_hw_debug_registers_save(ga->debug.registers);

	printk(KERN_NOTICE
	       "kkm_switch_to_host_kernel: segments ds %x es %x fs %x fsbase %lx gs %x gsbase %lx gskernbase %lx ss %x\n",
	       kkm_kontext->native_kernel_ds, kkm_kontext->native_kernel_es,
	       kkm_kontext->native_kernel_fs,
	       kkm_kontext->native_kernel_fs_base,
	       kkm_kontext->native_kernel_gs,
	       kkm_kontext->native_kernel_gs_base,
	       kkm_kontext->native_kernel_gs_kern_base,
	       kkm_kontext->native_kernel_ss);

	// flush TLB, and restore native kernel cr4
	__native_flush_tlb_global();

	// restore native kernel address space
	write_cr3(kkm_kontext->native_kernel_cr3);

	// restore native kernel segment registers
	loadsegment(ds, kkm_kontext->native_kernel_ds);
	loadsegment(es, kkm_kontext->native_kernel_es);

	loadsegment(fs, kkm_kontext->native_kernel_fs);
	wrmsrl(MSR_FS_BASE, kkm_kontext->native_kernel_fs_base);

	load_gs_index(kkm_kontext->native_kernel_gs);
	wrmsrl(MSR_GS_BASE, kkm_kontext->native_kernel_gs_base);
	wrmsrl(MSR_KERNEL_GS_BASE, kkm_kontext->native_kernel_gs_kern_base);

	loadsegment(ss, __KERNEL_DS);

	kkm_switch_to_hk_asm(kkm_kontext->guest_area);
}

void kkm_hw_debug_registers_save(uint64_t *registers)
{
#if 0
	uint64_t original_dr6 = 0;

	get_debugreg(registers[0], 0);
	get_debugreg(registers[1], 1);
	get_debugreg(registers[2], 2);
	get_debugreg(registers[3], 3);
	get_debugreg(registers[6], 6);
	get_debugreg(registers[7], 7);

	original_dr6 = registers[6];
	registers[6] &= 0x1E00F;

	printk(KERN_NOTICE
	       "kkm_hw_debug_registers_save: dr0 %llx dr1 %llx dr2 %llx dr3 %llx dr6 %llx masked_dr6 %llx dr7 %llx\n",
	       registers[0], registers[1], registers[2], registers[3],
	       original_dr6, registers[6], registers[7]);
#endif
}

void kkm_hw_debug_registers_restore(uint64_t *registers)
{
#if 0
	printk(KERN_NOTICE
	       "kkm_hw_debug_registers_restore: dr0 %llx dr1 %llx dr2 %llx dr3 %llx dr6 %llx dr7 %llx\n",
	       registers[0], registers[1], registers[2], registers[3],
	       registers[6], registers[7]);

	set_debugreg(registers[0], 0);
	set_debugreg(registers[1], 1);
	set_debugreg(registers[2], 2);
	set_debugreg(registers[3], 3);
	set_debugreg(registers[6], 6);
	set_debugreg(registers[7], 7);
#endif
}
