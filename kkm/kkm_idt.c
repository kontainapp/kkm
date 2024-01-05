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
#include <asm/traps.h>
#include <asm/io.h>

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_mm.h"
#include "kkm_kontext.h"
#include "kkm_guest_entry.h"
#include "kkm_guest_exit.h"
#include "kkm_intr_table.h"
#include "kkm_always.h"

/*
 * There is one idt system wide.
 * save the native kernel idt descriptor here
 * create kx idt and descriptor
 */
struct kkm_idt_entry {
	/*
	 * kx idt page and virtual address
	 */
	struct kkm_mmu_page_info idt;

	struct kkm_mmu_page_info idt_text_page0;
	struct kkm_mmu_page_info idt_text_page1;

	struct kkm_mmu_page_info kx_global;

	/*
	 * save native kernel idt descriptor
	 */
	struct desc_ptr native_idt_desc;

	/*
	 * kx idt descriptor
	 */
	struct desc_ptr guest_idt_desc;

	/*
	 * always idt and related memory
	 */
	struct kkm_mmu_page_info always_idt;
	struct kkm_mmu_page_info always_idt_native_ptrs;
	struct kkm_mmu_page_info always_idt_cpu_flags;
	struct kkm_mmu_page_info always_idt_unused;

	/*
	 * always idt desc
	 */
	struct desc_ptr always_idt_desc;
};

/*
 * save descriptors that are changed when we kx enter
 * kernel maintains separate copy of these descriptors in cea
 */
struct kkm_desc_entry {
	uint64_t last_id;
};

struct kkm_idt {
	int n_entries;

	struct kkm_idt_entry idt_entry;

	struct kkm_desc_entry desc_entries[NR_CPUS];

	/*
	 * save idt from each cpu
	 */
	struct desc_ptr kkm_saved_idt_desc[NR_CPUS];
};

struct kkm_idt kkm_idt;

int kkm_idt_descr_init(void)
{
	int ret_val = 0;
	struct kkm_idt_entry *idt_entry;
	int i = 0;
	struct gate_struct *gs;
	uint64_t intr_entry_addr = 0;

	if ((((void *)&kkm_intr_fill) - ((void *)&kkm_intr_entry_0)) >=
	    KKM_KX_INTR_CODE_SIZE) {
		printk(KERN_ERR "kkm_init: kx code overflow.\n");
		ret_val = -EINVAL;
		goto error;
	}

	if ((((void *)&kkm_guest_entry_end) -
	     ((void *)&kkm_switch_to_gp_asm)) >= KKM_KX_ENTRY_CODE_SIZE) {
		printk(KERN_ERR "kkm_init: kx code overflow.\n");
		ret_val = -EINVAL;
		goto error;
	}

	idt_entry = &kkm_idt.idt_entry;

	/*
	 * allocate KKM_IDT_ALLOCATION_PAGES pages
	 */
	ret_val = kkm_mm_allocate_pages(&idt_entry->idt.page,
					&idt_entry->idt.va, &idt_entry->idt.pa,
					KKM_IDT_ALLOCATION_PAGES);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_idt_descr_init: Failed to allocate memory for idt error(%d)\n",
		       ret_val);
		goto error;
	}

	/*
	 * covneniece variables to track various addresses
	 */
	idt_entry->idt_text_page0.va = idt_entry->idt.va + KKM_IDT_SIZE;
	idt_entry->idt_text_page0.pa =
		virt_to_phys(idt_entry->idt_text_page0.va);

	idt_entry->idt_text_page1.va = idt_entry->idt_text_page0.va + PAGE_SIZE;
	idt_entry->idt_text_page1.pa =
		virt_to_phys(idt_entry->idt_text_page1.va);

	idt_entry->kx_global.va = idt_entry->idt_text_page1.va + PAGE_SIZE;
	idt_entry->kx_global.pa = virt_to_phys(idt_entry->kx_global.va);

	idt_entry->always_idt.va = idt_entry->kx_global.va + PAGE_SIZE;
	idt_entry->always_idt.pa = virt_to_phys(idt_entry->always_idt.va);

	idt_entry->always_idt_native_ptrs.va =
		idt_entry->always_idt.va + PAGE_SIZE;
	idt_entry->always_idt_native_ptrs.pa =
		virt_to_phys(idt_entry->always_idt_native_ptrs.va);

	idt_entry->always_idt_cpu_flags.va =
		idt_entry->always_idt_native_ptrs.va + PAGE_SIZE;
	idt_entry->always_idt_cpu_flags.pa =
		virt_to_phys(idt_entry->always_idt_cpu_flags.va);

	idt_entry->always_idt_unused.va =
		idt_entry->always_idt_cpu_flags.va + PAGE_SIZE;
	idt_entry->always_idt_unused.pa =
		virt_to_phys(idt_entry->always_idt_unused.va);

	/*
	 * insert idt page, idt text and kx global in kx area
	 * idt in kx area is readonly
	 */
	kkm_mmu_set_kx_global_info(idt_entry->idt.pa,
				   idt_entry->idt_text_page0.pa,
				   idt_entry->idt_text_page1.pa,
				   idt_entry->kx_global.pa);

	/*
	 * save native kernel idt descriptor
	 */
	kkm_platform->kkm_store_idt(&idt_entry->native_idt_desc);

	if (idt_entry->native_idt_desc.size != (PAGE_SIZE - 1)) {
		printk(KERN_NOTICE
		       "kkm_idt_descr_init: idt size expecting 0xfff found 0x%x\n",
		       idt_entry->native_idt_desc.size);
	}

	printk(KERN_INFO "addr %px size %d\n",
	       (void *)idt_entry->native_idt_desc.address,
	       idt_entry->native_idt_desc.size);

	/*
	 * replace non standard handlers
	 */
	intr_function_pointers[X86_TRAP_DF] =
		(uint64_t)kkm_intr_entry_double_fault;
	intr_function_pointers[X86_TRAP_TS] =
		(uint64_t)kkm_intr_entry_invalid_TSS;
	intr_function_pointers[X86_TRAP_NP] =
		(uint64_t)kkm_intr_entry_segment_np;
	intr_function_pointers[X86_TRAP_SS] = (uint64_t)kkm_intr_entry_ss_fault;
	intr_function_pointers[X86_TRAP_GP] =
		(uint64_t)kkm_intr_entry_general_protection;
	intr_function_pointers[X86_TRAP_PF] =
		(uint64_t)kkm_intr_entry_page_fault;
	intr_function_pointers[X86_TRAP_AC] =
		(uint64_t)kkm_intr_entry_alignment_check;
	intr_function_pointers[X86_TRAP_CP] =
		(uint64_t)kkm_intr_entry_control_protection;
	intr_function_pointers[X86_TRAP_VC] =
		(uint64_t)kkm_intr_entry_vmm_communication;
	intr_function_pointers[X86_TRAP_SE] =
		(uint64_t)kkm_intr_entry_security_exception;

	/*
	 * initialize idt entries
	 * use kva to initialize idt, kx idt page is readonly
	 */
	gs = (struct gate_struct *)idt_entry->idt.va;
	for (i = 0; i < NR_VECTORS; i++) {
		intr_entry_addr = KKM_IDT_CODE_START_VA +
				  intr_function_pointers[i] -
				  intr_function_pointers[0];

		gs[i].offset_low = intr_entry_addr & 0xFFFF;
		gs[i].segment = __KERNEL_CS;
		gs[i].bits.ist = 0;
		gs[i].bits.zero = 0;
		gs[i].bits.type = GATE_INTERRUPT;
		gs[i].bits.dpl = 0;
		if (i == X86_TRAP_BP || i == X86_TRAP_OF) {
			gs[i].bits.dpl = 3;
		}
		gs[i].bits.p = 1;
		gs[i].offset_middle = (intr_entry_addr >> 16) & 0xFFFF;
		gs[i].offset_high = (intr_entry_addr >> 32) & 0xFFFFFFFF;
		gs[i].reserved = 0;
	}

	idt_entry->guest_idt_desc.size = idt_entry->native_idt_desc.size;
	/*
	 * use kx address mapping for kx idt
	 */
	idt_entry->guest_idt_desc.address = (unsigned long)kkm_mmu_get_idt_va();

	printk(KERN_INFO "guest addr %px size %d\n",
	       (void *)idt_entry->guest_idt_desc.address,
	       idt_entry->guest_idt_desc.size);

	/*
	 * copy interrupt entry code to kx area
	 */
	memcpy(idt_entry->idt_text_page0.va, kkm_intr_entry_0,
	       KKM_KX_INTR_CODE_SIZE);

	/*
	 * copy guest entry code to kx area
	 */
	memcpy(idt_entry->idt_text_page0.va + KKM_KX_INTR_CODE_SIZE,
	       kkm_switch_to_gp_asm, KKM_KX_ENTRY_CODE_SIZE);

	/*
	 * clear kx global area
	 */
	memset(idt_entry->kx_global.va, 0, KKM_IDT_GLOBAL_SIZE);

	/*
	 * set redirect pointer in kx_global area
	 */
	*(uint64_t *)idt_entry->kx_global.va =
		(uint64_t)kkm_switch_to_host_kernel;

	/*
	 * from native idt copy addresses of handlers to idt_native_ptrs
	 * These are stored from offset 0x0 in always_idt_native_ptrs.va
	 */
	gs = (struct gate_struct *)idt_entry->native_idt_desc.address;
	for (i = 0; i < NR_VECTORS; i++) {
		((uint64_t *)idt_entry->always_idt_native_ptrs.va)[i] =
			(uint64_t)gs[i].offset_high << 32 |
			(uint64_t)gs[i].offset_middle << 16 |
			(uint64_t)gs[i].offset_low;
	}

	/*
	 * from  guest idt copy address of handlers to idt_native_ptrs
	 * These are stored from offset 0x800 in always_idt_native_ptrs.va
	 */
	gs = (struct gate_struct *)idt_entry->idt.va;
	for (i = 0; i < NR_VECTORS; i++) {
		/*
		 * run handlers from normal driver load address
		 */
		((uint64_t *)idt_entry->always_idt_native_ptrs.va)[NR_VECTORS+i] =
			intr_function_pointers[i];
	}

	/*
	 * clear out cpu flags
	 * 64 bytes for each of 64 cpus
	 */
	memset((void*)idt_entry->always_idt_cpu_flags.va, 0, PAGE_SIZE);

	/*
	 * copy native idt to always_idt va
	 */
	memcpy((void *)idt_entry->always_idt.va,
	       (void *)idt_entry->native_idt_desc.address, PAGE_SIZE);

	/*
	 * insert always_idt function addresses to native idt
	 * keep rest of the descriptor flags intact
	 */
	gs = (struct gate_struct *)idt_entry->always_idt.va;
	for (i = 0; i < NR_VECTORS; i++) {
		intr_entry_addr = always_intr_function_pointers[i];

		gs[i].offset_low = intr_entry_addr & 0xFFFF;
		gs[i].offset_middle = (intr_entry_addr >> 16) & 0xFFFF;
		gs[i].offset_high = (intr_entry_addr >> 32) & 0xFFFFFFFF;
	}

	printk(KERN_NOTICE "always kx addresses %px %px %px %px\n",
	       (void *)KKM_ALWAYS_IDT_START, (void *)KKM_ALWAYS_IDT_PTRS_START,
	       (void *)KKM_ALWAYS_IDT_CPU_FLAGS_START, (void *)KKM_ALWAYS_IDT_UNUSED_START);
	printk(KERN_NOTICE "always kx sizes %px %px %px %px\n",
	       (void *)KKM_ALWAYS_IDT_SIZE, (void *)KKM_ALWAYS_IDT_PTRS_SIZE,
	       (void *)KKM_ALWAYS_IDT_PTRS_SIZE, (void *)KKM_ALWAYS_IDT_UNUSED_SIZE);

	/*
	 * insert idt page, idt text and kx global in kx area
	 * idt in kx area is readonly
	 */
	kkm_mmu_set_kx_always_global_info(idt_entry->always_idt.pa,
					  idt_entry->always_idt_native_ptrs.pa,
					  idt_entry->always_idt_cpu_flags.pa,
					  idt_entry->always_idt_unused.pa);

	/*
	 * use kx address mapping for kx idt
	 */
	idt_entry->always_idt_desc.size = idt_entry->native_idt_desc.size;
	idt_entry->always_idt_desc.address =
		(unsigned long)kkm_mmu_get_always_idt_va();

	printk(KERN_INFO "always idt desc addr %px size %d\n",
	       (void *)idt_entry->always_idt_desc.address,
	       idt_entry->always_idt_desc.size);
error:
	return ret_val;
}

int kkm_idt_init(void)
{
	int ret_val = 0;
	int i = 0;

	memset(&kkm_idt, 0, sizeof(kkm_idt));
	kkm_idt.n_entries = NR_CPUS;
	for (i = 0; i < NR_CPUS; i++) {
		kkm_idt.desc_entries[i].last_id = KKM_INVALID_ID;
	}

	if ((ret_val = kkm_idt_descr_init()) != 0) {
		printk(KERN_NOTICE "kkm_idt_init: failed to initialize idt\n");
		goto error;
	}

error:
	return ret_val;
}

void kkm_idt_cleanup(void)
{
	struct kkm_idt_entry *idt_entry;

	idt_entry = &kkm_idt.idt_entry;
	kkm_mm_free_pages(idt_entry->idt.va, KKM_IDT_ALLOCATION_PAGES);
	idt_entry->idt.page = NULL;
	idt_entry->idt.va = NULL;
}

int kkm_idt_get_desc(struct desc_ptr *native_desc, struct desc_ptr *guest_desc)
{
	struct kkm_idt_entry *idt_entry;

	idt_entry = &kkm_idt.idt_entry;

	//native_desc->size = idt_entry->native_idt_desc.size;
	//native_desc->address = idt_entry->native_idt_desc.address;

	kkm_platform->kkm_store_idt(native_desc);
	

	guest_desc->size = idt_entry->guest_idt_desc.size;
	guest_desc->address = idt_entry->guest_idt_desc.address;

	return 0;
}

void kkm_idt_set_id(int cpu, uint64_t id)
{
	kkm_idt.desc_entries[cpu].last_id = id;
}

uint64_t kkm_idt_get_id(int cpu)
{
	return kkm_idt.desc_entries[cpu].last_id;
}

void kkm_install_idt(int cpu_no)
{
	struct desc_ptr desc;

	kkm_platform->kkm_store_idt(&kkm_idt.kkm_saved_idt_desc[cpu_no]);
	printk(KERN_NOTICE "cpu %d prev idt sz %x addr %lx\n",
			cpu_no, kkm_idt.kkm_saved_idt_desc[cpu_no].size,
			kkm_idt.kkm_saved_idt_desc[cpu_no].address);

	kkm_platform->kkm_load_idt(&kkm_idt.idt_entry.always_idt_desc);

	local_irq_disable();

	kkm_platform->kkm_store_idt(&desc);

	local_irq_enable();
	printk(KERN_NOTICE "cpu %d prev idt sz %x addr %lx\n",
			cpu_no, desc.size, desc.address);
}

uint64_t kkm_idt_get_native_handler(int handler_no)
{
	uint64_t *idt_ptrs = (uint64_t *)kkm_idt.idt_entry.always_idt_native_ptrs.va;
	return idt_ptrs[handler_no];
}
