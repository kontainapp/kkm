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

#ifndef __KKM_MMU_H__
#define __KKM_MMU_H__

// clang-format off
/*
 *  memory map for one pml4 entry used for kkm private area
 *
 *           -------------------   0xFFFFFF0000000000ULL
 *           |      unused      |
 *           -------------------   0xFFFFFE8000200000ULL
 *           | CPU 63 area(16k) |
 *                   .
 *                   .
 *           | CPU 1 area(16k)  |
 *           --------------------  0xFFFFFE8000104000ULL
 *           | CPU 0 area(16k)  |
 *           --------------------  0xFFFFFE8000100000ULL
 *           |  Guest IDT       |
 *           --------------------  0xFFFFFE8000000000ULL
 *
 *   one pte of space allocated for guest private area
 *   4 pte's(4 * 4096 bytes) are assigned per physical cpu
 *   guest private area is accseed by indexing
 *   256 pte's are left for common entries
 */
// clang-format on

/* use unused kernel virtual address for kkm fixed mapping */
#define KKM_PRIVATE_START_VA (0xFFFFFE8000000000ULL)

/* pml4 table entry offset for KKM_PRIVATE_START_VA */
#define KKM_PGD_INDEX (509)

/* first page table entry index for physical cpu */
#define KKM_CPU_GA_INDEX_START (256)
/* number of page table entrie's for physical cpu */
#define KKM_PER_CPU_GA_PAGE_COUNT (4)

/* mask to remove incorrect flags */
#define KKM_PAGE_FLAGS_MASK (0x800000000000001FULL)
/* physical address mask to remove offset into page bits */
#define KKM_PAGE_PA_MASK (0xFFFFFFFFF000ULL)

/*
 * out of 512 entries in pml4
 *     0-255 are used for user memory and
 *     256-511 are used for kernel memory
 */

/* byte offset of first kernel pml4 entry(256 * 8) */
#define KKM_PGD_KERNEL_OFFSET (2048)
/* total bytes to be copied from native kernel pgd to kkm pgd(256 *8) */
#define KKM_PGD_KERNEL_SIZE (2048)

/*
 * guest physical memory is mapped from 16TB monitor virtual address
 * km uses a maximum of 512GB physical memory per guest
 * payload sees this memory offsetted from
 *     virtual address 0 for code growing up
 *     virtual address for stack growing down
 */
/* bytes offset into pml4 table for 16TB(monitor guest mapping) */
#define KKM_PGD_MONITOR_PAYLOAD_OFFSET (256)
/* byte offset into pml4 payload virtual address for code */
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_0 (0)
/* byte offset into pml4 payload virtual address for stack */
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_255 (255 * 8)
/* one pml4 entry */
#define KKM_PGD_PAYLOAD_SIZE (8)

/*
 * convenience structure for pages allocated for managing memory hierarchy
 */
struct kkm_mmu_page_info {
	struct page *page; /* page address */
	void *va; /* page virtual address */
	phys_addr_t pa; /* page physical address */
};

struct kkm_mmu {
	uint64_t pgd_entry; /* pml4 entry for kkm private area */
	struct kkm_mmu_page_info pud; /* page for pdpt */
	struct kkm_mmu_page_info pmd; /* page for pd */
	struct kkm_mmu_page_info pt; /* page for page table */
};

int kkm_mmu_init(void);
void kkm_mmu_cleanup(void);

uint64_t kkm_mmu_get_pgd_entry(void);
int kkm_mmu_get_per_cpu_start_index(void);
void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa,
			 uint64_t flags);
void kkm_mmu_set_guest_area(phys_addr_t pa0, phys_addr_t pa1, phys_addr_t pa2,
			    phys_addr_t pa3);
void *kkm_mmu_get_cur_cpu_guest_va(void);

void kkm_mmu_set_idt(void *idt_va);
void *kkm_mmu_get_idt_va(void);

int kkm_mmu_copy_kernel_pgd(struct kkm *kkm);
int kkm_mmu_sync(struct kkm *kkm);

#endif /* __KKM_MMU_H__ */
