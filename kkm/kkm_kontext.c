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
#include "kkm_run.h"
#include "kkm_offsets.h"
#include "kkm_intr.h"

DEFINE_PER_CPU(struct kkm_kontext *, current_kontext);

void kkm_hw_debug_registers_save(uint64_t *registers);
void kkm_hw_debug_registers_restore(uint64_t *registers);

/*
 * initialize context to execute payload
 */
int kkm_kontext_init(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;
	struct kkm_guest_area *ga = NULL;

	/*
	 * allocate guest private area
	 */
	ret_val = kkm_mm_allocate_pages(&kkm_kontext->guest_area_page,
					&kkm_kontext->guest_area, NULL,
					KKM_GUEST_AREA_PAGES);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontext_init: Failed to allocate memory for stack0 error(%d)\n",
		       ret_val);
		goto error;
	}

	/*
	 * get physical address of both pages allocated
	 */
	kkm_kontext->guest_area_page0_pa =
		virt_to_phys(kkm_kontext->guest_area);
	kkm_kontext->guest_area_page1_pa =
		virt_to_phys(kkm_kontext->guest_area + PAGE_SIZE);

	printk(KERN_NOTICE
	       "kkm_kontext_init: stack0 page %px va %px pa0 %llx pa1 %llx\n",
	       kkm_kontext->guest_area_page, kkm_kontext->guest_area,
	       kkm_kontext->guest_area_page0_pa,
	       kkm_kontext->guest_area_page1_pa);

	kkm_init_guest_area_redzone(
		(struct kkm_guest_area *)kkm_kontext->guest_area);

	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;
	/*
	 * save kva's in ga
	 * used when ga is mapped to kx area
	 */
	ga->kkm_kontext = kkm_kontext;
	ga->guest_area_beg = (uint64_t)ga;

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

/*
 * running in native kernel address space
 */
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;
	struct kkm_guest_area *ga =
		(struct kkm_guest_area *)kkm_kontext->guest_area;
	int cpu = -1;

	printk(KERN_NOTICE "kkm_kontext_switch_kernel:\n");

	/*
	 * setup physical cpu kontain area to this kontext guest area
	 */
	kkm_mmu_set_guest_area(kkm_kontext->guest_area_page0_pa,
			       kkm_kontext->guest_area_page1_pa,
			       (phys_addr_t)NULL, (phys_addr_t)NULL);

	/* do all kernel interaction before changing address space */
	/*
	 * fetch native and guest idt from cache
	 */
	kkm_idt_get_desc(&ga->native_idt_desc, &ga->guest_idt_desc);

	/*
	 * disable interrupts
	 */
	local_irq_disable();

	cpu = get_cpu();
	per_cpu(current_kontext, cpu) = kkm_kontext;
	printk(KERN_NOTICE "kkm_kontext_switch_kernel: cpu %d %px\n", cpu,
	       &cpu);

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: before %px %llx %llx %llx\n",
	       ga->kkm_kontext, ga->guest_area_beg, ga->native_kernel_stack,
	       ga->guest_stack_variable_address);

	/*
	 * save native kernel address space(cr3 and cr4)
	 */
	kkm_kontext->native_kernel_cr3 = __read_cr3();
	kkm_kontext->native_kernel_cr4 = __read_cr4();
	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: native kernel cr3 %lx cr4 %lx\n",
	       kkm_kontext->native_kernel_cr3, kkm_kontext->native_kernel_cr4);

	ga->guest_kernel_cr3 = kkm->guest_kernel_pa;
	ga->guest_payload_cr3 = kkm->guest_payload_pa;

	ga->guest_kernel_cr4 = __read_cr4();

	/*
	 * change to guest kernel address space
	 */
	kkm_change_address_space(ga->guest_kernel_cr3);

	/*
	 * save native kernel segment registers
	 */
	savesegment(ds, kkm_kontext->native_kernel_ds);
	savesegment(es, kkm_kontext->native_kernel_es);

	savesegment(fs, kkm_kontext->native_kernel_fs);
	rdmsrl(MSR_FS_BASE, kkm_kontext->native_kernel_fs_base);
	savesegment(gs, kkm_kontext->native_kernel_gs);
	rdmsrl(MSR_GS_BASE, kkm_kontext->native_kernel_gs_base);
	rdmsrl(MSR_KERNEL_GS_BASE, kkm_kontext->native_kernel_gs_kern_base);

	savesegment(ss, kkm_kontext->native_kernel_ss);

	/*
	 * save native kernel SYSCALL target address
	 */
	rdmsrl(MSR_LSTAR, kkm_kontext->native_kernel_entry_syscall_64);

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

	/*
	 * switch to guest kernel
	 * this code will switch stacks
	 */
	kkm_switch_to_gk_asm(ga, (uint64_t)ga->redzone_bottom);

	/* code is from intr/fault return path */
	kkm_hw_debug_registers_restore(kkm_kontext->native_debug_registers);

	printk(KERN_NOTICE
	       "kkm_kontext_switch_kernel: after %px %llx %llx %llx\n",
	       ga->kkm_kontext, ga->guest_area_beg, ga->native_kernel_stack,
	       ga->guest_stack_variable_address);

	printk(KERN_NOTICE "kkm_kontext_switch_kernel: ret_val %d %px\n",
	       ret_val, &ret_val);

	/*
	 * enable interrupts
	 */
	local_irq_enable();

	return ret_val;
}

/*
 * running in guest kernel address space
 * running on guest private area stack
 */
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga)
{
	int cpu = 0x66;
	struct cpu_entry_area *cea = NULL;
	uint64_t estack_start = 0;

	printk(KERN_NOTICE "kkm_guest_kernel_start_payload: ga %px\n", ga);
	ga = kkm_mmu_get_cur_cpu_guest_va();
	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: ga kontain private area %px\n",
	       ga);

	cpu = get_cpu();
	cea = get_cpu_entry_area(cpu);
	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: cpu %d %px cea %px\n",
	       cpu, &cpu, cea);

	ga->guest_stack_variable_address = (unsigned long long)&cpu;

	/*
	 * setup segments for switching to payload
	 */
	loadsegment(ds, 0);
	loadsegment(es, 0);

	loadsegment(fs, 0);
	wrmsrl(MSR_FS_BASE, ga->sregs.fs.base);

	/*
	 * set guest 64bit SYSCALL target address
	 */
	wrmsrl(MSR_LSTAR, (uint64_t)kkm_syscall_entry_asm);

	/*
	 * dont use km provided cs and ss, they control privilege
	 */
	ga->guest_payload_cs = __USER_CS;
	ga->guest_payload_ss = __USER_DS;

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: fsbase %llx usercs %llx userss %llx\n",
	       ga->sregs.fs.base, ga->guest_payload_cs, ga->guest_payload_ss);

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: rip %llx rsp %llx rflags %llx\n",
	       ga->regs.rip, ga->regs.rsp, ga->regs.rflags);

	/*
	 * flags are from userland
	 * make sure interrupts are enabled, iopl is 0 and resume flag is set
	 */
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

	/*
	 * verify stack redzone
	 */
	kkm_verify_guest_area_redzone(ga);

	/*
	 * save native kernel tss sp0 (intr stack)
	 */
	ga->native_save_tss_sp0 = cea->tss.x86_tss.sp0;

	/*
	 * ga is pointing to kx area
	 * replace tss stack 0 with payload_entry_stack,
	 * we can identify ga location from this.
	 */
	estack_start = (uint64_t)(&ga->redzone_bottom);
	load_sp0(estack_start);

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: swap sp0 native %llx guest %llx\n",
	       ga->native_save_tss_sp0, estack_start);

	/*
	 * interrupts are disbled at the begining of switch_kernel
	 * set new idt
	 */
	load_idt(&ga->guest_idt_desc);

	/*
	 * start payload
	 */
	kkm_switch_to_gp_asm(ga);

	/* NOTREACHED */
	/* never reaches, this code is used for debugging */

	printk(KERN_NOTICE
	       "kkm_guest_kernel_start_payload: returned from guest call\n");
}

/*
 * should be called from trap code, with zero context
 * enters with guest kernel cr3
 * running on guest stack
 */
void kkm_switch_to_host_kernel(void)
{
	int cpu = -1;
	struct cpu_entry_area *cea = NULL;
	struct kkm_kontext *kkm_kontext = NULL;
	struct kkm_guest_area *ga = NULL;

	cpu = get_cpu();
	cea = get_cpu_entry_area(cpu);
	kkm_kontext = per_cpu(current_kontext, cpu);
	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;

	printk(KERN_NOTICE
	       "kkm_switch_to_host_kernel: cpu %d stack address %px ga %px\n",
	       cpu, &cpu, ga);

	/*
	 * adjust registers from trap info
	 */
	ga->regs.rip = ga->trap_info.rip;
	ga->regs.rflags = ga->trap_info.rflags;

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

	/*
	 * restore native kernel tss sp0 (intr stack)
	 */
	load_sp0(ga->native_save_tss_sp0);

	/*
	 * restore native kernel idt
	 */
	load_idt(&ga->native_idt_desc);

	/*
	 * save native kernel SYSCALL target address
	 */
	wrmsrl(MSR_LSTAR, kkm_kontext->native_kernel_entry_syscall_64);

	/*
	 * restore native kernel segment registers
	 */
	loadsegment(ds, kkm_kontext->native_kernel_ds);
	loadsegment(es, kkm_kontext->native_kernel_es);

	loadsegment(fs, kkm_kontext->native_kernel_fs);
	wrmsrl(MSR_FS_BASE, kkm_kontext->native_kernel_fs_base);

	load_gs_index(kkm_kontext->native_kernel_gs);
	wrmsrl(MSR_GS_BASE, kkm_kontext->native_kernel_gs_base);
	wrmsrl(MSR_KERNEL_GS_BASE, kkm_kontext->native_kernel_gs_kern_base);

	loadsegment(ss, __KERNEL_DS);

	/*
	 * restore native kernel address space
	 * restore rest of the registers and switch stacks
	 */
	kkm_switch_to_hk_asm(kkm_kontext->native_kernel_cr3,
			     ((struct kkm_guest_area *)kkm_kontext->guest_area)
				     ->native_kernel_stack);
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
