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

#ifndef __KKM_H__
#define __KKM_H__

#include <linux/refcount.h>
#include <linux/uaccess.h>

#ifndef static_assert
#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)
#endif

#define KKM_REDZONE_CHECK_ENABLE (0)

#include "kkm_externs.h"
#include "kkm_ioctl.h"
#include "kkm_mmu.h"
#include "kkm_platform.h"

extern bool kkm_cpu_full_tlb_flush;

#define KKM_CONTEXT_MAP_PAGE_COUNT (3)
#define KKM_CONTEXT_MAP_SIZE (KKM_CONTEXT_MAP_PAGE_COUNT * 4096)

#define KKM_INVALID_ID (-1ULL)
/*
 * maximum number of times same trap is allowed to repreat.
 */
#define KKM_MAX_REPEAT_TRAP (16)

/*
 * maximum xsave area allocated
 * one for kernel and one for payload
 */
#define KKM_FPU_XSAVE_ALLOC_PAGES (2)
#define KKM_FPU_XSAVE_ALLOC_SIZE (PAGE_SIZE)

extern void (*kkm_fpu_save_xstate)(void *);
extern void (*kkm_fpu_restore_xstate)(void *);

extern struct kkm_platform_calls *kkm_platform;

struct kkm_kontext_mmap_area {
	struct page *page;
	unsigned long kvaddr;
	int offset;
};

/*
 * per vcpu context area
 * used to save some information
 * required to return to native kernel
 */
struct kkm_kontext {
	uint64_t id;
	uint64_t index;
	bool used;
	bool first_thread;
	bool new_thread;
	/*
	 * save/restore of debug only if the cureent context
	 * is using hardware debug functionality
	 */
	bool debug_registers_set;
	int kontext_fd;
	struct task_struct
		*task; /* kernel task associated with this kontain kontext */
	struct kkm *kkm; /* back pointer to kontainer */

	struct kkm_kontext_mmap_area mmap_area[KKM_CONTEXT_MAP_PAGE_COUNT];

	/*
	 * guest private area
	 */
	struct page *guest_area_page; /* guest area page pointer */
	void *guest_area; /* virtual address of guest area */
	phys_addr_t
		guest_area_page0_pa; /* physical address of page 0 of guest private area */
	phys_addr_t
		guest_area_page1_pa; /* physical address of page 1 of guest private area */

	/*
	 * XSAVE page. First 2k for kernel and next 2k for payload
	 */
	struct kkm_mmu_page_info xsave;
	void *kkm_kernel_xsave;
	void *kkm_payload_xsave;
	bool valid_payload_xsave_area;

	/*
	 * saved during switch to guest kernel.
	 * restore during switch back to native kernel
	 */
	uint64_t native_kernel_cr3; /* native kernel cr3 values, not the same as pml4 pointer */
	uint64_t native_kernel_cr4; /* native kernel cr4 */

	uint16_t native_kernel_ds; /* native kernel ds */
	uint16_t native_kernel_es; /* native kernel es */

	uint16_t native_kernel_fs; /* native kernel fs */
	uint64_t native_kernel_fs_base; /* native kernel fs base TLS */

	uint16_t native_kernel_gs; /* native kernel gs */
	uint64_t native_kernel_gs_base; /* native kernel gs base */
	uint64_t native_kernel_gs_kern_base; /* native kernel gs base when in user mode */

	uint16_t native_kernel_ss; /* native kernel ss */

	uint64_t native_kernel_entry_syscall_64; /* native kernel 64bit syscall entry point */

	uint64_t native_debug_registers[8];

	struct desc_ptr native_gdt_descr; /* native gdt */
	uint64_t native_tr; /* native task register */

	/*
	 * syscall related stuff
	 */
	bool syscall_pending;
	uint64_t ret_val_mva;

	/*
	 * need to save and restore rbx
	 * to accomodate km
	 */
	bool exception_posted;
	uint64_t exception_saved_rax;
	uint64_t exception_saved_rbx;

	/*
	 * current trap info
	 */
	uint64_t trap_addr;
	uint64_t error_code;

	/*
	 * paranoid entry check
	 * save previous trap
	 */
	uint64_t prev_trap_no;
	uint64_t prev_trap_addr;
	uint64_t prev_error_code;
	uint64_t trap_repeat_counter;
};

struct kkm_mem_slot {
	bool used;
	uint64_t npages;
	uint64_t user_pfn;
	struct kkm_memory_region mr;
};

/*
 * per guest data structure
 */
struct kkm {
	uint64_t id;
	int kontainer_fd;
	refcount_t reference_count;

	struct mm_struct *mm; /* kernel address space pointer */

	/*
	 * physical memory management
	 */
	struct mutex mem_lock;
	uint32_t mem_slot_count;
	struct kkm_mem_slot mem_slot[KKM_MAX_MEMORY_SLOTS];

	uint64_t id_map_addr;

	/*
	 * kontext pointers
	 */
	struct mutex kontext_lock;
	uint32_t kontext_count;
	struct kkm_kontext *kontext[KKM_MAX_CONTEXTS];

	/*
	 * page fault lock
	 * process only one page fault per mm
	 */
	struct mutex pf_lock;

	/*
	 * guest kernel pgd page
	 */
	struct kkm_mmu_page_info gk_pgd;

	/*
	 * guest payload pgd page
	 */
	struct kkm_mmu_page_info gp_pgd;

	/*
	 * guest kernel p4d page
	 */
	struct kkm_mmu_page_info gk_p4d;

	/*
	 * guest payload p4d page
	 */
	struct kkm_mmu_page_info gp_p4d;

	/*
	 * page for low address
	 * same table is used for kernel and payload p4d for low addresses
	 */
	struct kkm_mmu_page_info low_p4d;

	/*
	 * guest private area page table hierarchy
	 * vdso, vvar and monitor -> guest code
	 */
	struct kkm_mmu_pml4e kkm_guest_pml4e;
};

#endif /* __KKM_H__ */
