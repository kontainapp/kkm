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
#include <asm/desc.h>
#include <asm/tlbflush.h>
#include <asm/debugreg.h>
#include <asm/cpu_entry_area.h>

#include "kkm.h"
#include "kkm_kontext.h"
#include "kkm_mm.h"
#include "kkm_mmu.h"
#include "kkm_misc.h"
#include "kkm_entry.h"
#include "kkm_idt_cache.h"

DEFINE_PER_CPU(struct kkm_kontext *, current_kontext);

void kkm_hw_debug_registers_save(uint64_t *registers);
void kkm_hw_debug_registers_restore(uint64_t *registers);

int kkm_kontext_init(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	// stack0
	ret_val = kkm_mm_allocate_pages(&kkm_kontext->guest_area_page,
					&kkm_kontext->guest_area, NULL,
					KKM_GUEST_AREA_PAGES);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontext_init: Failed to allocate memory for stack0 error(%d)\n",
		       ret_val);
		goto error;
	}

	kkm_kontext->guest_area_page0_pa =
		virt_to_phys(kkm_kontext->guest_area);
	kkm_kontext->guest_area_page1_pa =
		virt_to_phys(kkm_kontext->guest_area + PAGE_SIZE);

	printk(KERN_NOTICE
	       "kkm_kontext_init: stack0 page %lx va %lx pa0 %llx pa1 %llx\n",
	       (unsigned long)kkm_kontext->guest_area_page,
	       (unsigned long)kkm_kontext->guest_area,
	       kkm_kontext->guest_area_page0_pa,
	       kkm_kontext->guest_area_page1_pa);

	kkm_init_guest_area_redzone(
		(struct kkm_guest_area *)kkm_kontext->guest_area);

#if 0
	// store_gdt not available in aws kernels
	store_gdt(&kkm_kontext->native_gdt_descr);
	printk(KERN_NOTICE "kkm_kontainer_init: native kernel gdt size %x base %lx\n",
	       kkm_kontext->native_gdt_descr.size, kkm_kontext->native_gdt_descr.address);
#endif

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
	struct desc_ptr *native_idt_desc = NULL;
	struct desc_ptr *guest_idt_desc = NULL;
	struct task_struct *tsk = current;

#if 1
	// delete
	uint64_t efer = 0;
	uint64_t star = 0;
	uint64_t lstar = 0;

	rdmsrl(MSR_EFER, efer);
	rdmsrl(MSR_STAR, star);
	rdmsrl(MSR_LSTAR, lstar);

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: EFER %llx STAR %llx LSTAR %llx stack %lx %lx\n",
	       efer, star, lstar, (unsigned long)tsk->stack,
	       (unsigned long)&efer);
#endif
	printk(KERN_NOTICE "kkm_kontext_switch_kernel:\n");

	// setup physical cpu kontain area to this kontex guest area
	kkm_mmu_set_guest_area(kkm_kontext->guest_area_page0_pa,
			       kkm_kontext->guest_area_page1_pa,
			       (phys_addr_t)NULL, (phys_addr_t)NULL);

	// do all kernel interaction before changing address space
	kkm_idt_get_desc(&native_idt_desc, &guest_idt_desc);
	ga->native_idt.size = native_idt_desc->size;
	ga->native_idt.address = native_idt_desc->address;

	// insert idt entry at specific va
	kkm_mmu_set_idt((void *)guest_idt_desc->address);
	ga->guest_idt.size = guest_idt_desc->size;
#if 0
	ga->guest_idt.address = (unsigned long)kkm_mmu_get_idt_va();
#else
	ga->guest_idt.address = native_idt_desc->address;
#endif

	// disable interrupts
	local_irq_disable();

	cpu = get_cpu();
	per_cpu(current_kontext, cpu) = kkm_kontext;
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: cpu %d %llx\n", cpu,
	       (unsigned long long)&cpu);

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: before %llx %llx %llx %llx\n",
	       (unsigned long long)ga->kkm_kontext, ga->guest_area_beg,
	       ga->native_kernel_stack, ga->guest_stack_variable_address);

	// save trampoline stack
	cea = get_cpu_entry_area(cpu);
	memcpy(&ga->native_entry_stack, &cea->entry_stack_page.stack,
	       sizeof(struct entry_stack));

	// save native kernel address space
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	kkm_kontext->native_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: native kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->native_kernel_cr3, kkm_kontext->native_kernel_cr4);

	ga->guest_kernel_cr3 = kkm->guest_kernel_pa;
	ga->guest_payload_cr3 = kkm->guest_payload_pa;

	ga->guest_kernel_cr4 = __read_cr4();

	// change to guest kernel address space
	kkm_change_address_space(ga->guest_kernel_cr3);


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

	kkm_hw_debug_registers_save(kkm_kontext->native_debug_registers);

	kkm_switch_to_gk_asm(ga, kkm_kontext,
			     (unsigned long long)ga->redzone_bottom);

	kkm_hw_debug_registers_restore(kkm_kontext->native_debug_registers);

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

	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: ga %llx\n", (unsigned long long)ga);
	ga = kkm_mmu_get_cur_cpu_guest_va();
	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: ga kontain private area %llx\n", (unsigned long long)ga);

	cpu = get_cpu();
	cea = get_cpu_entry_area(cpu);
	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: cpu %d %llx cea %llx\n",
	       cpu, (unsigned long long)&cpu, (unsigned long long)cea);

	ga->guest_stack_variable_address = (unsigned long long)&cpu;

	loadsegment(ds, 0);
	loadsegment(es, 0);

	loadsegment(fs, 0);
	wrmsrl(MSR_FS_BASE, ga->sregs.fs.base);

	ga->guest_payload_cs = __USER_CS;
	ga->guest_payload_ss = __USER_DS;

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: fsbase %llx usercs %llx userss %llx\n",
	       ga->sregs.fs.base, ga->guest_payload_cs, ga->guest_payload_ss);

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: rip %llx rsp %llx rflags %llx\n",
	       ga->regs.rip, ga->regs.rsp, ga->regs.rflags);

	// flags are from userland
	// make sure interrupts are enabled, iopl is 0 and resume flag is set
	if ((ga->regs.rflags & X86_EFLAGS_IF) == 0) {
		printk(KERN_NOTICE
		       "kkm_guest_kernel_start_payload: interrupts are disabled in rflags, enabling\n");
		// keep interrupts disabled till trap handlers are completely working
		// ga->regs.rflags |= X86_EFLAGS_IF;
	}
	// TODO: delete this
	ga->regs.rflags &= ~(X86_EFLAGS_IF);
	if ((ga->regs.rflags & X86_EFLAGS_IOPL) != 0) {
		printk(KERN_NOTICE
		       "kkm_guest_kernel_start_payload: user provided iopl 0x%llx, changing to 0\n",
		       (ga->regs.rflags & X86_EFLAGS_IOPL) >>
			       X86_EFLAGS_IOPL_BIT);
		ga->regs.rflags &= ~(X86_EFLAGS_IOPL);
	}
	if ((ga->regs.rflags & X86_EFLAGS_RF) == 0) {
		printk(KERN_NOTICE
		       "kkm_guest_kernel_start_payload: resume flag is not set rflags, enabling\n");
		ga->regs.rflags |= X86_EFLAGS_RF;
	}

	kkm_hw_debug_registers_restore(ga->debug.registers);

	// interrupts are disbled at the begining of switch_kernel
	// set new idt
	load_idt(&ga->guest_idt);

	// switch to guest payload address space is done in assembly
	// just before switching to user space
	// TODO: move flush to assembly
	// flush TLB
	kkm_flush_tlb_all();

	// start payload
	kkm_switch_to_gp_asm(ga);

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: returned from guest call\n");

	cea = get_cpu_entry_area(cpu);
	memcpy(&ga->payload_entry_stack, &cea->entry_stack_page.stack,
	       sizeof(struct entry_stack));

	kkm_trap_entry_asm();
	//kkm_switch_to_host_kernel();
}

// should be called from trap code, with zero context
// enters with guest kernel cr3
void kkm_switch_to_host_kernel(void)
{
	int cpu = -1;
	struct kkm_kontext *kkm_kontext = NULL;
	struct kkm_guest_area *ga = NULL;

	cpu = get_cpu();
	kkm_kontext = per_cpu(current_kontext, cpu);
	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;

	printk(KERN_NOTICE "kkm_switch_to_host_kernel: cpu %d stack address %llx ga %llx\n", cpu,
	       (unsigned long long)&cpu, (unsigned long long)ga);

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

	// restore native kernel address space
	kkm_change_address_space(kkm_kontext->native_kernel_cr3);

	// restore native kernel idt
	load_idt(&ga->native_idt);

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
