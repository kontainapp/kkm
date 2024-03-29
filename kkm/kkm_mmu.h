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

/*
 * use 2 pcids that are not used by linux kernel
 */
#define PCID_MASK (0xFFFULL)
#define GUEST_KERNEL_PCID (0x7FULL)
#define GUEST_PAYLOAD_PCID (0xFFULL)

/* use unused kernel virtual address for kkm fixed mapping */
#define KKM_PRIVATE_START_VA (0xFFFFFE8000000000ULL)

/* index in PGD for KX P4D when 5 level page tables are used */
#define KKM_KX_PGD_P4D_INDEX (511)

/* pml4 table entry offset for KKM_PRIVATE_START_VA */
#define KKM_KX_PGD_INDEX (509)

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
#define KKM_PGD_LOW_P4D_ENTRY (0)
/* bytes offset into pml4 table for 16TB(monitor guest mapping) */
#ifndef KM_GPA_AT_16T
#define KKM_PGD_MONITOR_PAYLOAD_ENTRY (0)
#else
#define KKM_PGD_MONITOR_PAYLOAD_ENTRY (32)
#endif
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

/*
 * For each pml4 entry maintain all pages here
 * this currently allows only for one child for each parent entry
 * Lowest level entry(page table) can have as many entries.
 */
struct kkm_mmu_pml4e {
	uint64_t pgd_entry; /* pml4 entry for kkm private area */
	struct kkm_mmu_page_info pud; /* page for pdpt */
	struct kkm_mmu_page_info pmd; /* page for pd */
	struct kkm_mmu_page_info pt; /* page for page table */
};

int kkm_mmu_init(void);
void kkm_mmu_cleanup(void);
void kkm_mmu_flush_tlb(void);
void kkm_mmu_flush_tlb_one_page(uint64_t addr);
int kkm_create_pml4(struct kkm_mmu_pml4e *kmu, uint64_t address);
void kkm_cleanup_pml4(struct kkm_mmu_pml4e *kmu);

void kkm_mmu_set_guest_area(int cpu_index, phys_addr_t pa0, phys_addr_t pa1,
			    phys_addr_t pa2, phys_addr_t pa3);
void *kkm_mmu_get_cur_cpu_guest_va(int cpu_index);

void *kkm_mmu_get_idt_va(void);
void kkm_mmu_set_kx_global_info(phys_addr_t idt_pa, phys_addr_t text_page0_pa,
				phys_addr_t text_page1_pa,
				phys_addr_t kx_global_pa);

int kkm_mmu_copy_kernel_pgd(uint64_t current_pgd_base, void *guest_kernel_va,
			    void *guest_payload_va, void *kernel_p4d_va,
			    void *payload_p4d_va);
int kkm_mmu_sync(uint64_t current_pgd_base, void *guest_kernel_va,
		 void *guest_payload_va, struct kkm_mmu_pml4e *guest,
		 void *low_p4d_va, phys_addr_t low_p4d_pa);
bool kkm_mmu_update_priv_area(uint64_t guest_fault_address,
			      uint64_t monitor_fault_address,
			      uint64_t current_pgd_base,
			      struct kkm_mmu_pml4e *guest);

#endif /* __KKM_MMU_H__ */
