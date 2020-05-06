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
 *                   .
 *                   .
 *           --------------------  0xFFFFFE8000004000ULL
 *      RW   |  redirect addr   |
 *           --------------------  0xFFFFFE8000003000ULL
 *      RWX  |  guest entry 2k  |
 *           --------------------  0xFFFFFE8000002800ULL
 *      RWX  |  INTR entry 6k   |
 *           --------------------  0xFFFFFE8000001000ULL
 *      R    |  Guest IDT       |
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

/* page table indices start */

#define KKM_PTE_INDEX_IDT (0)
#define KKM_PTE_INDEX_TEXT_0 (1)
#define KKM_PTE_INDEX_TEXT_1 (2)
#define KKM_PTE_INDEX_KX (3)

/* first page table entry index for physical cpu */
#define KKM_CPU_GA_INDEX_START (256)
/* number of page table entrie's for physical cpu */
#define KKM_PER_CPU_GA_PAGE_COUNT (4)

/* page table indices END */

/* mask to remove incorrect flags */
#define KKM_PAGE_FLAGS_MASK (0x800000000000001FULL)
/* physical address mask to remove offset into page bits */
#define KKM_PAGE_PA_MASK (0xFFFFFFFFF000ULL)

/* IDT address start */

/*
 * First page for IDT mapped Read only
 * second and thirt for code mapped Execute enable
 *     - first page 256 * 16 bytes per entry
 *     - second page rest of the entry code
 * fourth page for global data mapped Read Write
 */
#define KKM_IDT_TABLE (1)
#define KKM_IDT_TEXT (2)
#define KKM_IDT_GLBL (1)

#define KKM_IDT_ALLOCATION_PAGES (KKM_IDT_TABLE + KKM_IDT_TEXT + KKM_IDT_GLBL)

/* IDT table start address */
#define KKM_IDT_START_VA (KKM_PRIVATE_START_VA)
/* IDT table size */
#define KKM_IDT_SIZE (KKM_IDT_TABLE * PAGE_SIZE)
/* kx code start address */
#define KKM_IDT_CODE_START_VA (KKM_IDT_START_VA + KKM_IDT_SIZE)
/* kx code size */
#define KKM_IDT_CODE_SIZE (KKM_IDT_TEXT * PAGE_SIZE)
/* kx global data start address */
#define KKM_IDT_GLOBAL_START (KKM_IDT_CODE_START_VA + KKM_IDT_CODE_SIZE)
/* kx global data size */
#define KKM_IDT_GLOBAL_SIZE (KKM_IDT_GLBL * PAGE_SIZE)

/* 16 bytes is code generated for each intr entry */
#define KKM_IDT_ENTRY_FUNCTION_SIZE (16)

/*
 * Maximum allowed kx exit code size
 */
#define KKM_KX_INTR_CODE_SIZE (6 * 1024)

/*
 * Maximum allowed kx entry code size
 */
#define KKM_KX_ENTRY_CODE_SIZE (2 * 1024)

/*
 * kx exit code start addr
 */
#define KKM_KX_INTR_CODE_START_ADDR (KKM_IDT_CODE_START_VA)

/*
 * kx entry code start addr
 */
#define KKM_KX_ENTRY_CODE_START_ADDR                                           \
	(KKM_KX_INTR_CODE_START_ADDR + KKM_KX_INTR_CODE_SIZE)

/* IDT address end */

/* PML4 convenience macros start */

/*
 * out of 512 entries in pml4
 *     0-255 are used for user memory and
 *     256-511 are used for kernel memory
 */
/* each entry in PML4 corresponds to 512GB */

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
#define KKM_PGD_MONITOR_PAYLOAD_ENTRY (32)
#define KKM_PGD_MONITOR_PAYLOAD_ENTRY_OFFSET (KKM_PGD_MONITOR_PAYLOAD_ENTRY * 8)
/* byte offset into pml4 payload virtual address for code */
#define KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY (0)
#define KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY_OFFSET                              \
	(KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY * 8)
/* byte offset into pml4 payload virtual address for stack */
#define KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY (255)
#define KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY_OFFSET                                 \
	(KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY * 8)
/* one pml4 entry */
#define KKM_PGD_PAYLOAD_SIZE (8)

/* PML4 convenience macros end */

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
int kkm_create_p4ml(struct kkm_mmu *kmu, uint64_t address);
void kkm_cleanup_p4ml(struct kkm_mmu *kmu);

uint64_t kkm_mmu_get_pgd_entry(void);
int kkm_mmu_get_per_cpu_start_index(void);
void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa,
			 uint64_t flags);
void kkm_mmu_set_guest_area(phys_addr_t pa0, phys_addr_t pa1, phys_addr_t pa2,
			    phys_addr_t pa3);
void *kkm_mmu_get_cur_cpu_guest_va(void);

void kkm_mmu_set_idt(phys_addr_t idt_pa);
void *kkm_mmu_get_idt_va(void);
void kkm_mmu_set_idt_text(phys_addr_t text_page0_pa, phys_addr_t text_page1_pa);
void kkm_mmu_set_kx_global(phys_addr_t kx_global_pa);

int kkm_mmu_copy_kernel_pgd(uint64_t current_pgd_base, uint64_t guest_kernel_va,
			    uint64_t guest_payload_va);
int kkm_mmu_sync(uint64_t current_pgd_base, uint64_t guest_kernel_va,
		 uint64_t guest_payload_va, struct kkm_mmu *guest);
bool kkm_kontext_mmu_update_priv_area(uint64_t guest_fault_address,
				      uint64_t monitor_fault_address,
				      uint64_t current_pgd_base,
				      struct kkm_mmu *guest);
bool kkm_kontext_mmu_get_table_va(uint64_t *table_va, int index);

#endif /* __KKM_MMU_H__ */
