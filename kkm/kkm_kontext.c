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
#include <linux/moduleparam.h>
#include <asm/desc.h>
#include <asm/tlbflush.h>
#include <asm/debugreg.h>
#include <asm/cpu_entry_area.h>
#include <asm/traps.h>
#include <asm/io.h>

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_kontext.h"
#include "kkm_mm.h"
#include "kkm_misc.h"
#include "kkm_guest_entry.h"
#include "kkm_guest_exit.h"
#include "kkm_idt_cache.h"
#include "kkm_ioctl.h"
#include "kkm_offsets.h"
#include "kkm_intr.h"
#include "kkm_intr_table.h"

static bool __read_mostly lazy_flush_tlb = true;
module_param(lazy_flush_tlb, bool, S_IRUGO | S_IWUSR);

static bool __read_mostly log_failed_guest_va_translate = true;
module_param(log_failed_guest_va_translate, bool, S_IRUGO | S_IWUSR);

static bool __read_mostly log_failed_page_fault_handling = true;
module_param(log_failed_page_fault_handling, bool, S_IRUGO | S_IWUSR);

DEFINE_PER_CPU(struct kkm_kontext *, current_kontext);

/*
 * function pointer to guest payload entry code in kx area
 */
void (*kkm_switch_to_gp_asm_func_ptr)(struct kkm_guest_area *ga) =
	(void (*)(struct kkm_guest_area *ga))KKM_KX_ENTRY_CODE_START_ADDR;

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
		       "kkm_kontext_init: Thread %llx failed to allocate memory for stack0 error(%d)\n",
		       kkm_kontext->id, ret_val);
		goto error;
	}

	/*
	 * get physical address of both pages allocated
	 */
	kkm_kontext->guest_area_page0_pa =
		virt_to_phys(kkm_kontext->guest_area);
	kkm_kontext->guest_area_page1_pa =
		virt_to_phys(kkm_kontext->guest_area + PAGE_SIZE);

	kkm_init_guest_area_redzone(
		(struct kkm_guest_area *)kkm_kontext->guest_area);

	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;
	/*
	 * save kva's in ga
	 * used when ga is mapped to kx area
	 */
	ga->kkm_kontext = kkm_kontext;
	ga->guest_area_beg = (uint64_t)ga;
	ga->cpu = KKM_INVALID_CPU_ID;

	kkm_kontext->new_thread = true;
	kkm_kontext->debug_registers_set = false;

	kkm_kontext->syscall_pending = false;
	kkm_kontext->ret_val_mva = -1;

	kkm_kontext->exception_posted = false;
	kkm_kontext->exception_saved_rbx = -1;

error:
	if (ret_val != 0) {
		kkm_kontext_cleanup(kkm_kontext);
	}
	return ret_val;
}

void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext)
{
	if (kkm_kontext->guest_area_page != NULL) {
		kkm_mm_free_pages(kkm_kontext->guest_area,
				  KKM_GUEST_AREA_PAGES);
		kkm_kontext->guest_area_page = NULL;
		kkm_kontext->guest_area = NULL;
	}
}

int kkm_kontext_reinit(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	kkm_kontext->new_thread = true;
	kkm_kontext->debug_registers_set = false;

	kkm_kontext->syscall_pending = false;
	kkm_kontext->ret_val_mva = -1;

	return ret_val;
}

/*
 * running in native kernel address space
 */
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext)
{
	struct kkm *kkm = kkm_kontext->kkm;
	int ret_val = 0;
	struct kkm_guest_area *ga = NULL;
	int cpu = -1;
	uint64_t syscall_ret_value = 0;
	struct kkm_run *kkm_run = NULL;
	struct kkm_private_area *pa = NULL;

	ga = (struct kkm_guest_area *)kkm_kontext->guest_area;

	kkm_run = (struct kkm_run *)kkm_kontext->mmap_area[0].kvaddr;
	if (kkm_run->immediate_exit == 1) {
		goto error;
	}

	/*
	 * new thread, adjust stack if this is started by clone.
	 */
	if (kkm_kontext->new_thread == true) {
		kkm_kontext->new_thread = false;
		if (kkm_kontext->first_thread == false) {
			pa = (struct kkm_private_area *)kkm_kontext
				     ->mmap_area[1]
				     .kvaddr;
			if (pa->reason == FAULT_SYSCALL) {
				ga->regs.rsp += KKM_KM_HC_ARGS_SIZE;
			}
		}
	}

	if (kkm_kontext->syscall_pending == true) {
		/*
		 * copy system call return value from monitor
		 */
		if (copy_from_user(&syscall_ret_value,
				   (void *)kkm_kontext->ret_val_mva,
				   sizeof(uint64_t))) {
			ret_val = -EFAULT;
			goto error;
		}
		ga->regs.rax = syscall_ret_value;
	}

	kkm_kontext->syscall_pending = false;
	kkm_kontext->ret_val_mva = -1;

	if (kkm_kontext->exception_posted == true) {
		ga->regs.rbx = kkm_kontext->exception_saved_rbx;
	}

	kkm_kontext->exception_posted = false;
	kkm_kontext->exception_saved_rbx = -1;

	kkm_kontext->prev_trap_no = -1;
	kkm_kontext->prev_trap_addr = -1;
	kkm_kontext->prev_error_code = -1;
	kkm_kontext->trap_repeat_counter = -1;

begin:
	if (signal_pending(current) != 0) {
		kkm_run->exit_reason = KKM_EXIT_UNKNOWN;
		ret_val = -EINTR;
		goto error;
	}
	ret_val = 0;
	cpu = -1;

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

	if ((lazy_flush_tlb == false) || (cpu != ga->cpu) ||
	    (kkm->id != kkm_idt_get_id(cpu))) {
		/*
		 * invalidate older TLB entries
		 */
		kkm_mmu_flush_tlb();

		ga->cpu = cpu;
		kkm_idt_set_id(cpu, kkm->id);
	}

	/*
	 * save native kernel address space(cr3 and cr4)
	 */
	kkm_kontext->native_kernel_cr3 = kkm_platform->kkm_read_cr3();
	kkm_kontext->native_kernel_cr4 = kkm_platform->kkm_read_cr4();

	ga->guest_kernel_cr3 =
		(kkm->gk_pml4.pa & ~PCID_MASK) | GUEST_KERNEL_PCID;
	ga->guest_payload_cr3 =
		(kkm->gp_pml4.pa & ~PCID_MASK) | GUEST_PAYLOAD_PCID;

	ga->guest_kernel_cr4 = kkm_kontext->native_kernel_cr4;

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

	if (kkm_kontext->debug_registers_set == true) {
		kkm_hw_debug_registers_save(
			kkm_kontext->native_debug_registers);
	}

	ga->intr_no = -1;

	/*
	 * switch to guest kernel
	 * this code will switch stacks
	 */
	kkm_switch_to_gk_asm(ga, (uint64_t)ga->redzone_bottom);

	/* code is from intr/fault return path */
	if (kkm_kontext->debug_registers_set == true) {
		kkm_hw_debug_registers_restore(
			kkm_kontext->native_debug_registers);
	}

	/*
	 * enable interrupts
	 */
	local_irq_enable();

	ret_val = kkm_process_intr(kkm_kontext);
	if (ret_val == KKM_KONTEXT_FAULT_PROCESS_DONE) {
		if (ga->intr_no == LOCAL_TIMER_VECTOR) {
			schedule();
		}
		if (ga->intr_no == X86_TRAP_PF &&
		    kkm_kontext->prev_trap_no == X86_TRAP_PF &&
		    kkm_kontext->prev_trap_addr == kkm_kontext->trap_addr &&
		    kkm_kontext->prev_error_code == kkm_kontext->error_code) {
			kkm_kontext->trap_repeat_counter++;
			printk(KERN_NOTICE
			       "kkm_kontext_switch_kernel: Thread %llx repeat page fault at the same address %llx error code %llx count %llx\n",
			       kkm_kontext->id, kkm_kontext->trap_addr,
			       kkm_kontext->error_code,
			       kkm_kontext->trap_repeat_counter);
			if (kkm_kontext->trap_repeat_counter >
			    KKM_MAX_REPEAT_TRAP) {
				printk(KERN_NOTICE
				       "kkm_kontext_switch_kernel: Thread %llx bailing out after allowed repeat page faults at the same address %llx error code %llx\n",
				       kkm_kontext->id, kkm_kontext->trap_addr,
				       kkm_kontext->error_code);
				ret_val = -EFAULT;
				goto error;
			}
			goto begin;
		}
		kkm_kontext->prev_trap_no = ga->intr_no;
		kkm_kontext->prev_trap_addr = kkm_kontext->trap_addr;
		kkm_kontext->prev_error_code = kkm_kontext->error_code;
		kkm_kontext->trap_repeat_counter = 0;
		goto begin;
	}

error:
	return ret_val;
}

/*
 * running in guest kernel address space
 * running on guest private area stack
 */
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga)
{
	int cpu = -1;
	struct cpu_entry_area *cea = NULL;
	uint64_t estack_start = 0;
	uint64_t syscall_entry_addr = 0;
	struct kkm_kontext *kkm_kontext = ga->kkm_kontext;

	cpu = ga->cpu;
	cea = get_cpu_entry_area(cpu);

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
	syscall_entry_addr = kkm_syscall_entry_asm - kkm_intr_entry_0 +
			     KKM_IDT_CODE_START_VA;
	wrmsrl(MSR_LSTAR, syscall_entry_addr);

	/*
	 * dont use km provided cs and ss, they control privilege
	 */
	ga->guest_payload_cs = __USER_CS;
	ga->guest_payload_ss = __USER_DS;

	/*
	 * flags are from userland
	 * make sure interrupts are enabled, iopl is 0 and resume flag is set
	 */
	if ((ga->regs.rflags & X86_EFLAGS_IF) == 0) {
		ga->regs.rflags |= X86_EFLAGS_IF;
	}
	if ((ga->regs.rflags & X86_EFLAGS_IOPL) != 0) {
		ga->regs.rflags &= ~(X86_EFLAGS_IOPL);
	}
	if ((ga->regs.rflags & X86_EFLAGS_RF) == 0) {
		ga->regs.rflags |= X86_EFLAGS_RF;
	}

	/*
	 * single step is requested, set required flags
	 */
	if ((ga->debug.control &
	     (KKM_GUESTDBG_SINGLESTEP | KKM_GUESTDBG_ENABLE)) ==
	    (KKM_GUESTDBG_SINGLESTEP | KKM_GUESTDBG_ENABLE)) {
		ga->regs.rflags |= X86_EFLAGS_TF;
	} else {
		ga->regs.rflags &= ~X86_EFLAGS_TF;
	}

	if (kkm_kontext->debug_registers_set == true) {
		kkm_hw_debug_registers_restore(ga->debug.registers);
	}

	/*
	 * verify stack redzone
	 */
	kkm_verify_guest_area_redzone(ga);

	/*
	 * save native kernel tss sp0 (intr stack)
	 */
	ga->native_save_tss_sp0 = cea->tss.x86_tss.sp0;
	ga->native_save_tss_sp1 = cea->tss.x86_tss.sp1;
	ga->native_save_tss_sp2 = cea->tss.x86_tss.sp2;

	/*
	 * ga is pointing to kx area
	 * replace tss stack 0 with payload_entry_stack,
	 * we can identify ga location from this.
	 */
	estack_start = (uint64_t)(&ga->redzone_bottom);
	load_sp0(estack_start);

	/*
	 * interrupts are disbled at the begining of switch_kernel
	 * set new idt
	 */
	kkm_platform->kkm_load_idt(&ga->guest_idt_desc);

	/*
	 * start payload
	 * call function in kx area
	 */
	//kkm_switch_to_gp_asm_func_ptr(ga);
	kkm_switch_to_gp_asm(ga);

	/* NOTREACHED */
}

/*
 * should be called from trap code, with zero context
 * enters with guest kernel cr3
 * running on guest stack
 */
void kkm_switch_to_host_kernel(struct kkm_guest_area *ga)
{
	struct kkm_kontext *kkm_kontext = NULL;

	kkm_kontext = ga->kkm_kontext;

	/*
	 * adjust registers from trap info
	 */
	ga->regs.rip = ga->trap_info.rip;
	ga->regs.rflags = ga->trap_info.rflags;

	if (kkm_kontext->debug_registers_set == true) {
		kkm_hw_debug_registers_save(ga->debug.registers);
	}

	/*
	 * restore native kernel tss sp0 (intr stack)
	 */
	load_sp0(ga->native_save_tss_sp0);
	this_cpu_write(cpu_tss_rw.x86_tss.sp1, ga->native_save_tss_sp1);
	this_cpu_write(cpu_tss_rw.x86_tss.sp2, ga->native_save_tss_sp2);

	/*
	 * restore native kernel idt
	 */
	kkm_platform->kkm_load_idt(&ga->native_idt_desc);

	/*
	 * restore native kernel SYSCALL target address
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
	get_debugreg(registers[0], 0);
	get_debugreg(registers[1], 1);
	get_debugreg(registers[2], 2);
	get_debugreg(registers[3], 3);
	get_debugreg(registers[6], 6);
	get_debugreg(registers[7], 7);

	registers[6] &= 0x1E00FULL;
}

void kkm_hw_debug_registers_restore(uint64_t *registers)
{
	set_debugreg(registers[0], 0);
	set_debugreg(registers[1], 1);
	set_debugreg(registers[2], 2);
	set_debugreg(registers[3], 3);
	set_debugreg(registers[6], 6);
	set_debugreg(registers[7], 7);
}

/*
 * trap/intr specific monitor constants
 * keep in sync with km
 */

#define KKM_HYPERCALL_IO_PORT_BASE (0x8000)
#define KKM_HYPERCALL_IO_SIZE (4)
#define KKM_HYPERCALL_IO_COUNT (1)
#define KKM_EXCEPTION_IO_PORT (0x81FD)

int kkm_process_intr(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;
	struct kkm_guest_area *ga =
		(struct kkm_guest_area *)kkm_kontext->guest_area;
	struct kkm_run *kkm_run = NULL;

	kkm_run = (struct kkm_run *)kkm_kontext->mmap_area[0].kvaddr;
	kkm_run->exit_reason = KKM_EXIT_UNKNOWN;

	if (ga->intr_no == X86_TRAP_GP) {
		ret_val = kkm_process_general_protection(kkm_kontext, ga,
							 kkm_run);
	} else if (ga->intr_no == X86_TRAP_PF) {
		ret_val = kkm_process_page_fault(kkm_kontext, ga, kkm_run);
	} else {
		switch (ga->intr_no) {
		case X86_TRAP_DE:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case X86_TRAP_DB:
			ret_val = kkm_process_debug(kkm_kontext, ga, kkm_run);
			break;
		case X86_TRAP_NMI:
			break;
		case X86_TRAP_BP:
			ret_val = kkm_process_breakpoint(kkm_kontext, ga,
							 kkm_run);
			break;
		case X86_TRAP_OF:
		case X86_TRAP_BR:
		case X86_TRAP_UD:
		case X86_TRAP_NM:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case X86_TRAP_DF:
			ret_val = kkm_process_common_with_error(kkm_kontext, ga,
								kkm_run);
			break;
		case X86_TRAP_OLD_MF:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case X86_TRAP_TS:
		case X86_TRAP_NP:
		case X86_TRAP_SS:
			ret_val = kkm_process_common_with_error(kkm_kontext, ga,
								kkm_run);
			break;
		case X86_TRAP_GP:
			ret_val = kkm_process_general_protection(kkm_kontext,
								 ga, kkm_run);
			break;
		case X86_TRAP_PF:
			ret_val = kkm_process_page_fault(kkm_kontext, ga,
							 kkm_run);
			break;
		case X86_TRAP_SPURIOUS:
		case X86_TRAP_MF:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case X86_TRAP_AC:
			ret_val = kkm_process_common_with_error(kkm_kontext, ga,
								kkm_run);
			break;
		case X86_TRAP_MC:
		case X86_TRAP_XF:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case KKM_INTR_SYSCALL:
			ret_val = kkm_process_syscall(kkm_kontext, ga, kkm_run);
			break;
		case X86_TRAP_VC:
			ret_val = kkm_process_common_without_error(kkm_kontext,
								   ga, kkm_run);
			break;
		case X86_TRAP_SE:
			ret_val = kkm_process_common_with_error(kkm_kontext, ga,
								kkm_run);
			break;
		default:
			kkm_forward_intr(intr_forward_pointers[ga->intr_no]);
			ret_val = KKM_KONTEXT_FAULT_PROCESS_DONE;
			break;
		}
	}

	return ret_val;
}

void kkm_setup_hypercall(struct kkm_kontext *kkm_kontext,
			 struct kkm_guest_area *ga, struct kkm_run *kkm_run,
			 uint16_t port, uint32_t addr, enum fault_reason reason)
{
	struct kkm_private_area *pa = NULL;

	kkm_run->exit_reason = KKM_EXIT_IO;
	kkm_run->io.direction = KKM_EXIT_IO_OUT;
	kkm_run->io.size = KKM_HYPERCALL_IO_SIZE;
	kkm_run->io.port = port | KKM_HYPERCALL_IO_PORT_BASE;
	kkm_run->io.count = KKM_HYPERCALL_IO_COUNT;
	kkm_run->io.data_offset = PAGE_SIZE;
	pa = (struct kkm_private_area *)kkm_kontext->mmap_area[1].kvaddr;
	pa->data = addr;
	pa->reason = reason;
}

/*
 * processing differed to monitor
 * record exception and return to monitor
 */
int kkm_process_common_without_error(struct kkm_kontext *kkm_kontext,
				     struct kkm_guest_area *ga,
				     struct kkm_run *kkm_run)
{
	int ret_val = 0;
	struct kkm_intr_stack_no_error_code args;
	uint64_t gva = 0;
	uint64_t mva = 0;

	args.rax = ga->regs.rax;
	args.rbx = ga->regs.rbx;
	args.rdx = ga->regs.rdx;
	args.rip = ga->trap_info.rip;
	args.cs = ga->trap_info.cs;
	args.rflags = ga->trap_info.rflags;
	args.rsp = ga->trap_info.rsp;
	args.ss = ga->trap_info.ss;

	ga->regs.rsp -= sizeof(struct kkm_intr_stack_no_error_code);
	gva = ga->regs.rsp;

	if (kkm_guest_va_to_monitor_va(kkm_kontext, gva, &mva, NULL) == false) {
		ret_val = -EFAULT;
		goto error;
	}
	if (copy_to_user((void *)mva, &args,
			 sizeof(struct kkm_intr_stack_no_error_code))) {
		ret_val = -EFAULT;
		goto error;
	}

	kkm_setup_hypercall(kkm_kontext, ga, kkm_run, KKM_EXCEPTION_IO_PORT,
			    ga->regs.rsp, FAULT_UNKNOWN);

	kkm_kontext->exception_posted = true;
	kkm_kontext->exception_saved_rbx = ga->regs.rbx;
	ga->regs.rbx = ga->intr_no;

error:
	return ret_val;
}

/*
 * processing differed to monitor
 * record exception and return to monitor
 */
int kkm_process_common_with_error(struct kkm_kontext *kkm_kontext,
				  struct kkm_guest_area *ga,
				  struct kkm_run *kkm_run)
{
	int ret_val = 0;
	struct kkm_intr_stack_with_error_code args;
	uint64_t gva = 0;
	uint64_t mva = 0;

	args.rax = ga->regs.rax;
	args.rbx = ga->regs.rbx;
	args.rdx = ga->regs.rdx;
	args.error = ga->trap_info.error;
	args.rip = ga->trap_info.rip;
	args.cs = ga->trap_info.cs;
	args.rflags = ga->trap_info.rflags;
	args.rsp = ga->trap_info.rsp;
	args.ss = ga->trap_info.ss;

	ga->regs.rsp -= sizeof(struct kkm_intr_stack_with_error_code);

	gva = ga->regs.rsp;

	if (kkm_guest_va_to_monitor_va(kkm_kontext, gva, &mva, NULL) == false) {
		ret_val = -EFAULT;
		goto error;
	}
	if (copy_to_user((void *)mva, &args,
			 sizeof(struct kkm_intr_stack_with_error_code))) {
		ret_val = -EFAULT;
		goto error;
	}

	kkm_setup_hypercall(kkm_kontext, ga, kkm_run, KKM_EXCEPTION_IO_PORT,
			    ga->regs.rsp, FAULT_UNKNOWN);

	kkm_kontext->exception_posted = true;
	kkm_kontext->exception_saved_rbx = ga->regs.rbx;
	ga->regs.rbx = ga->intr_no;

error:
	return ret_val;
}

int kkm_process_debug(struct kkm_kontext *kkm_kontext,
		      struct kkm_guest_area *ga, struct kkm_run *kkm_run)
{
	kkm_run->exit_reason = KKM_EXIT_DEBUG;
	kkm_run->debug.arch.exception = X86_TRAP_DB;
	kkm_run->debug.arch.pc = ga->trap_info.rip;
	kkm_run->debug.arch.dr6 = ga->debug.registers[6];
	kkm_run->debug.arch.dr7 = ga->debug.registers[7];

	return 0;
}

int kkm_process_breakpoint(struct kkm_kontext *kkm_kontext,
			   struct kkm_guest_area *ga, struct kkm_run *kkm_run)
{
	kkm_run->exit_reason = KKM_EXIT_DEBUG;
	kkm_run->debug.arch.exception = X86_TRAP_BP;
	ga->regs.rip -= 1;
	kkm_run->debug.arch.pc = ga->regs.rip;

	return 0;
}

int kkm_process_general_protection(struct kkm_kontext *kkm_kontext,
				   struct kkm_guest_area *ga,
				   struct kkm_run *kkm_run)
{
	int ret_val = 0;
	uint64_t monitor_fault_address = 0;

	/*
	 * convert guest address to monitor address
	 */
	if (kkm_guest_va_to_monitor_va(kkm_kontext, ga->trap_info.rip,
				       &monitor_fault_address, NULL) == false) {
		ret_val = -EFAULT;
		goto error;
	}

	/*
	 * fetch offending instruction byte
	 */
	if (copy_from_user(ga->instruction_decode,
			   (void *)monitor_fault_address, sizeof(uint8_t))) {
		ret_val = -EFAULT;
		goto error;
	}

	if (ga->instruction_decode[0] == KKM_OUT_OPCODE) {
		kkm_setup_hypercall(kkm_kontext, ga, kkm_run, ga->regs.rdx,
				    ga->regs.rax, FAULT_HYPER_CALL);
		ga->regs.rip += 1;
	}

error:
	return ret_val;
}

int kkm_process_page_fault(struct kkm_kontext *kkm_kontext,
			   struct kkm_guest_area *ga, struct kkm_run *kkm_run)
{
	int ret_val = 0;
	uint64_t error_code = ga->trap_info.error;
	uint64_t monitor_fault_address = 0;
	bool priv_area = false;
	struct kkm *kkm = kkm_kontext->kkm;

	kkm_kontext->trap_addr = ga->sregs.cr2;
	kkm_kontext->error_code = ga->trap_info.error;

	mutex_lock(&kkm_kontext->kkm->pf_lock);

	/*
	 * convert guest address to monitor address
	 */
	if (kkm_guest_va_to_monitor_va(kkm_kontext, ga->sregs.cr2,
				       &monitor_fault_address,
				       &priv_area) == false) {
		ret_val = -EFAULT;
		goto error;
	}

	if ((error_code & X86_PF_USER) == X86_PF_USER) {
		/*
		 * copy 1 bytes from monitor virtual address
		 * this will trigger native kernel page fault
		 */
		if (copy_from_user(ga->instruction_decode,
				   (void *)monitor_fault_address,
				   sizeof(uint8_t))) {
			ret_val = -EFAULT;
			goto error;
		}

		if ((error_code & X86_PF_WRITE) == X86_PF_WRITE) {
			if (copy_to_user((void *)monitor_fault_address,
					 ga->instruction_decode,
					 sizeof(uint8_t))) {
				ret_val = -EFAULT;
				goto error;
			}
		}

		/*
		 * this fault is non linear map area
		 * we need to update page tables correctly
		 */
		if (priv_area == true) {
			kkm_kontext_mmu_update_priv_area(ga->sregs.cr2,
							 monitor_fault_address,
							 (uint64_t)kkm->mm->pgd,
							 &kkm->kkm_guest_pml4e);
		}

		/*
		 * invalidate this address from TLB
		 * this is required as we are no longer flusing TLB on re-entry
		 */
		kkm_mmu_flush_tlb_one_page(ga->sregs.cr2);

		ret_val = KKM_KONTEXT_FAULT_PROCESS_DONE;

		/*
		 * page fault is completely resolved
		 * clear fault address
		 */
		ga->sregs.cr2 = 0;
	} else {
		printk(KERN_NOTICE
		       "kkm_process_page_fault: Thread %llx page fault from kernel thread %d\n",
		       kkm_kontext->id, kkm_kontext->kontext_fd);
		kkm_show_trap_info(ga);
	}

error:

	if ((ret_val && ret_val != KKM_KONTEXT_FAULT_PROCESS_DONE) &&
	    (log_failed_page_fault_handling == true)) {
		printk(KERN_NOTICE
		       "kkm_process_page_fault: Thread %llx ret_val %d %llx\n",
		       kkm_kontext->id, ret_val, kkm_kontext->trap_addr);
	}
	mutex_unlock(&kkm_kontext->kkm->pf_lock);

	return ret_val;
}

int kkm_process_syscall(struct kkm_kontext *kkm_kontext,
			struct kkm_guest_area *ga, struct kkm_run *kkm_run)
{
	int ret_val = 0;
	struct kkm_hc_args args;
	uint64_t gva = 0;
	uint64_t mva = 0;

	gva = ga->regs.rsp - sizeof(struct kkm_hc_args);

	kkm_setup_hypercall(kkm_kontext, ga, kkm_run, ga->regs.rax, gva,
			    FAULT_SYSCALL);

	args.ret_val = 0;
	args.argument1 = ga->regs.rdi;
	args.argument2 = ga->regs.rsi;
	args.argument3 = ga->regs.rdx;
	args.argument4 = ga->regs.r10;
	args.argument5 = ga->regs.r8;
	args.argument6 = ga->regs.r9;

	if (kkm_guest_va_to_monitor_va(kkm_kontext, gva, &mva, NULL) == false) {
		ret_val = -EFAULT;
		goto error;
	}
	if (copy_to_user((void *)mva, &args, sizeof(struct kkm_hc_args))) {
		ret_val = -EFAULT;
		goto error;
	}

	kkm_kontext->syscall_pending = true;
	kkm_kontext->ret_val_mva = mva;
	ga->regs.rax = gva;

error:
	return ret_val;
}

bool kkm_guest_va_to_monitor_va(struct kkm_kontext *kkm_kontext,
				uint64_t guest_va, uint64_t *monitor_va,
				bool *priv_area)
{
	struct kkm_mem_slot *mem_slot = NULL;
	bool ret_val = false;

	*monitor_va = 0;
	if (priv_area != NULL) {
		*priv_area = false;
	}

	/*
	 * bottom va text
	 */
	if (guest_va >= KKM_GUEST_MEM_START_VA &&
	    guest_va < KKM_GUEST_MAX_PHYS_MEM) {
		*monitor_va = KKM_KM_USER_MEM_BASE + guest_va;
		ret_val = true;
		goto end;
	}

	/*
	 * top va stacks + mmap
	 */
	if (guest_va >= KKM_GUEST_VA_OFFSET &&
	    guest_va < KKM_GUEST_MEM_TOP_VA) {
		*monitor_va =
			KKM_KM_USER_MEM_BASE + (guest_va - KKM_GUEST_VA_OFFSET);
		ret_val = true;
		goto end;
	}

	/*
	 * vdso + vvar
	 */
	mem_slot = &kkm_kontext->kkm->mem_slot[KKM_KM_RSRV_VDSOSLOT];
	if (mem_slot->used == true && guest_va >= KKM_GUEST_VVAR_VDSO_BASE_VA &&
	    guest_va <
		    (KKM_GUEST_VVAR_VDSO_BASE_VA + mem_slot->mr.memory_size)) {
		*monitor_va = guest_va - KKM_GUEST_VVAR_VDSO_BASE_VA +
			      mem_slot->mr.userspace_addr;
		if (priv_area != NULL) {
			*priv_area = true;
		}
		ret_val = true;
		goto end;
	}

	/*
	 * guest mem
	 */
	mem_slot = &kkm_kontext->kkm->mem_slot[KKM_KM_RSRV_KMGUESTMEM_SLOT];
	if (mem_slot->used == true &&
	    guest_va >= KKM_GUEST_KMGUESTMEM_BASE_VA &&
	    guest_va <
		    (KKM_GUEST_KMGUESTMEM_BASE_VA + mem_slot->mr.memory_size)) {
		*monitor_va = guest_va - KKM_GUEST_KMGUESTMEM_BASE_VA +
			      mem_slot->mr.userspace_addr;
		if (priv_area != NULL) {
			*priv_area = true;
		}
		ret_val = true;
		goto end;
	}

end:
	if ((ret_val == false) && (log_failed_guest_va_translate == true)) {
		printk(KERN_NOTICE
		       "kkm_guest_va_to_monitor_va: Thread %llx failed translation faulted guest va %llx monitor va %llx ret_val %d\n",
		       kkm_kontext->id, guest_va, *monitor_va, ret_val);
	}

	return ret_val;
}
