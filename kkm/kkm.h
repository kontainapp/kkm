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

#ifndef __KKM_H__
#define __KKM_H__

#include "kkm_ioctl.h"

#define KKM_CONTEXT_MAP_PAGE_COUNT (3)
#define KKM_CONTEXT_MAP_SIZE (KKM_CONTEXT_MAP_PAGE_COUNT * 4096)

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
	bool used;
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
	 * saved during switch to guest kernel.
	 * restore during switch back to native kernel
	 */
	unsigned long
		native_kernel_cr3; /* native kernel cr3 values, not the same as pml4 pointer */
	unsigned long native_kernel_cr4; /* native kernel cr4 */

	unsigned short native_kernel_ds; /* native kernel ds */
	unsigned short native_kernel_es; /* native kernel es */

	unsigned short native_kernel_fs; /* native kernel fs */
	unsigned long native_kernel_fs_base; /* native kernel fs base TLS */

	unsigned short native_kernel_gs; /* native kernel gs */
	unsigned long native_kernel_gs_base; /* native kernel gs base */
	unsigned long
		native_kernel_gs_kern_base; /* native kernel gs base when in user mode */

	unsigned short native_kernel_ss; /* native kernel ss */

	uint64_t native_kernel_entry_syscall_64; /* native kernel 64bit syscall entry point */

	uint64_t native_debug_registers[8];

	struct desc_ptr native_gdt_descr; /* native gdt */
	uint64_t native_tr; /* native task register */

	char scratch_buffer[256];
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
	struct kkm_kontext kontext[KKM_MAX_CONTEXTS];

	/*
	 * guest kernel pml4 page
	 */
	struct page *guest_kernel_page;
	unsigned long guest_kernel_va;
	phys_addr_t guest_kernel_pa;

	/*
	 * guest payload pml4 page
	 */
	struct page *guest_payload_page;
	unsigned long guest_payload_va;
	phys_addr_t guest_payload_pa;
};

#endif /* __KKM_H__ */
