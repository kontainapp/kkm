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

#define	KKM_PRIVATE_START_VA (0xFFFFFE8000000000ULL)
#define KKM_PGD_INDEX	(509)

#define KKM_CPU_GA_INDEX_START	(256)
#define KKM_PER_CPU_GA_PAGE_COUNT	(4)

#define	KKM_PAGE_FLAGS_MASK	(0x800000000000001FULL)
#define	KKM_PAGE_PA_MASK	(0xFFFFFFFFF000ULL)


#define KKM_PGD_KERNEL_OFFSET (2048)
#define KKM_PGD_KERNEL_SIZE (2048)

#define KKM_PGD_MONITOR_PAYLOAD_OFFSET (256)
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_0 (0)
#define KKM_PGD_GUEST_PAYLOAD_OFFSET_255 (255 * 8)
#define KKM_PGD_PAYLOAD_SIZE (8)

struct kkm_mmu_page_info {
	struct page *page;
	void *va;
	phys_addr_t pa;
};

struct kkm_mmu {
	uint64_t pgd_entry;
	struct kkm_mmu_page_info pud;
	struct kkm_mmu_page_info pmd;
	struct kkm_mmu_page_info pt;
};

int kkm_mmu_init(void);
void kkm_mmu_cleanup(void);

uint64_t kkm_mmu_get_pgd_entry(void);
int kkm_mmu_get_per_cpu_start_index(void);
void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa, uint64_t flags);
void kkm_mmu_set_guest_area(phys_addr_t pa0, phys_addr_t pa1, phys_addr_t pa2, phys_addr_t pa3);
void *kkm_mmu_get_cur_cpu_guest_va(void);

void kkm_mmu_set_idt(void *idt_va);
void *kkm_mmu_get_idt_va(void);

int kkm_mmu_copy_kernel_pgd(struct kkm *kkm);
int kkm_mmu_sync(struct kkm *kkm);

#endif /* __KKM_MMU_H__ */
