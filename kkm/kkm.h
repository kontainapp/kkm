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

struct kkm_kontext {
	bool used;
	int kontext_fd;
	struct task_struct *task;
	struct kkm *kkm;

	struct kkm_kontext_mmap_area mmap_area[KKM_CONTEXT_MAP_PAGE_COUNT];

	struct page *guest_area_page;
	void *guest_area;

	unsigned long native_kernel_cr3;
	unsigned long guest_kernel_cr3;

	char scratch_buffer[256];
};

struct kkm_mem_slot {
	bool used;
	uint64_t npages;
	uint64_t user_pfn;
	struct kkm_memory_region mr;
};

struct kkm {
	int kontainer_fd;
	refcount_t reference_count;

	struct mm_struct *mm;

	struct mutex mem_lock;
	uint32_t mem_slot_count;
	struct kkm_mem_slot mem_slot[KKM_MAX_MEMORY_SLOTS];

	uint64_t id_map_addr;

	struct mutex kontext_lock;
	uint32_t kontext_count;
	struct kkm_kontext kontext[KKM_MAX_CONTEXTS];

	struct page *guest_kernel_page;
	unsigned long guest_kernel;
	phys_addr_t guest_kernel_pa;

	struct page *guest_payload_page;
	unsigned long guest_payload;
	phys_addr_t guest_payload_pa;

	struct page *idt_page;
	void *idt;
};

#endif /* __KKM_H__ */
