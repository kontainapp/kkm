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
#include <linux/log2.h>

#include "kkm.h"
#include "kkm_mm.h"
#include "kkm_mmu.h"

struct kkm_mmu kkm_mmu;

/*
 * allocate pages and initialize pud, pmd, pt for private area
 */
int kkm_mmu_init(void)
{
	int ret_val = 0;

	memset(&kkm_mmu, 0, sizeof(struct kkm_mmu));

	/* alocate page for pud */
	ret_val = kkm_mm_allocate_page(&kkm_mmu.pud.page, &kkm_mmu.pud.va,
				       &kkm_mmu.pud.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_mmu_init: failed to allocate pud page error(%d)\n",
		       ret_val);
		goto error;
	}
	/* alocate page for pmd */
	ret_val = kkm_mm_allocate_page(&kkm_mmu.pmd.page, &kkm_mmu.pmd.va,
				       &kkm_mmu.pmd.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_mmu_init: failed to allocate pmd page error(%d)\n",
		       ret_val);
		goto error;
	}
	/* alocate page for pt */
	ret_val = kkm_mm_allocate_page(&kkm_mmu.pt.page, &kkm_mmu.pt.va,
				       &kkm_mmu.pt.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_mmu_init: failed to allocate pt page error(%d)\n",
		       ret_val);
		goto error;
	}

	/* pages are allocated and zeroed, __GFP_ZERO flag is used to allocate page */

	/* createp pgd entry */
	kkm_mmu.pgd_entry = (kkm_mmu.pud.pa & KKM_PAGE_PA_MASK) | _PAGE_USER |
			    _PAGE_RW | _PAGE_PRESENT;

	/* initialize first entry in pud */
	kkm_mmu_insert_page(kkm_mmu.pud.va, 0, kkm_mmu.pmd.pa,
			    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

	/* initialize first entry in pmd */
	kkm_mmu_insert_page(kkm_mmu.pmd.va, 0, kkm_mmu.pt.pa,
			    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

error:
	if (ret_val != 0) {
		kkm_mmu_cleanup();
	}
	return ret_val;
}

void kkm_mmu_cleanup(void)
{
	if (kkm_mmu.pud.page != NULL) {
		free_page((unsigned long long)kkm_mmu.pud.va);
	}
	if (kkm_mmu.pmd.page != NULL) {
		free_page((unsigned long long)kkm_mmu.pmd.va);
	}
	if (kkm_mmu.pt.page != NULL) {
		free_page((unsigned long long)kkm_mmu.pt.va);
	}
}

/*
 * return pml4 entry for private memory area
 */
uint64_t kkm_mmu_get_pgd_entry(void)
{
	return kkm_mmu.pgd_entry;
}

/*
 * return current cpu private area first page table index
 * next KKM_PER_CPU_GA_PAGE_COUNT belong to this physical cpu
 */
int kkm_mmu_get_per_cpu_start_index(void)
{
	int cpu = get_cpu();
	int page_index =
		KKM_CPU_GA_INDEX_START + cpu * KKM_PER_CPU_GA_PAGE_COUNT;
	return page_index;
}

/*
 * set one page table entry
 */
void kkm_mmu_set_entry(void *pt_va, int index, uint64_t entry)
{
	((uint64_t *)pt_va)[index] = entry;
}

/*
 * set one page table entry with given physical address and flags
 */
void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa, uint64_t flags)
{
	printk(KERN_NOTICE
	       "kkm_mmu_insert_page: pt va %px index %x pa %llx flags %llx\n",
	       pt_va, index, pa, flags);
	((uint64_t *)pt_va)[index] =
		(pa & KKM_PAGE_PA_MASK) | (flags & KKM_PAGE_FLAGS_MASK);
}

/*
 * set this physicl cpu private area page table entries
 */
void kkm_mmu_set_guest_area(phys_addr_t pa0, phys_addr_t pa1, phys_addr_t pa2,
			    phys_addr_t pa3)
{
	int page_index = kkm_mmu_get_per_cpu_start_index();
	uint64_t flags = _PAGE_NX | _PAGE_RW | _PAGE_PRESENT;

	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index, pa0, flags);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 1, pa1, flags);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 2, pa2, flags);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 3, pa3, flags);
}

/*
 * return start virtual address of this physical cpu guest private address
 */
void *kkm_mmu_get_cur_cpu_guest_va(void)
{
	int page_index = kkm_mmu_get_per_cpu_start_index();
	uint64_t va = KKM_PRIVATE_START_VA + page_index * PAGE_SIZE;

	return (void *)va;
}

/*
 * insert idt page at KKM_IDT_START_VA
 */
void kkm_mmu_set_idt(phys_addr_t idt_pa)
{
	/*
	 * pte KKM_PTE_INDEX_IDT corresponds to KKM_IDT_START_VA
	 */
	kkm_mmu_insert_page(kkm_mmu.pt.va, KKM_PTE_INDEX_IDT, idt_pa,
			    _PAGE_NX | _PAGE_PRESENT);
}

void *kkm_mmu_get_idt_va(void)
{
	return (void *)KKM_PRIVATE_START_VA;
}

/*
 * insert idt page at KKM_IDT_CODE_START_VA
 */
void kkm_mmu_set_idt_text(phys_addr_t text_page0_pa, phys_addr_t text_page1_pa)
{
	/*
	 * pte KKM_PTE_INDEX_TEXT_0 corresponds to KKM_IDT_GLOBAL_START
	 */
	kkm_mmu_insert_page(kkm_mmu.pt.va, KKM_PTE_INDEX_TEXT_0, text_page0_pa,
			    _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu.pt.va, KKM_PTE_INDEX_TEXT_1, text_page1_pa,
			    _PAGE_RW | _PAGE_PRESENT);
}

/*
 * insert idt page at KKM_IDT_GLOBAL_START
 */
void kkm_mmu_set_kx_global(phys_addr_t kx_global_pa)
{
	/*
	 * pte KKM_PTE_INDEX_KX corresponds to KKM_IDT_GLOBAL_START
	 */
	kkm_mmu_insert_page(kkm_mmu.pt.va, KKM_PTE_INDEX_KX, kx_global_pa,
			    _PAGE_NX | _PAGE_RW | _PAGE_PRESENT);
}

/*
 * copy range of bytes from one area to other
 */
static void kkm_mmu_copy_range(uint64_t src_base, uint64_t src_offset,
			       uint64_t dest_base, uint64_t dest_offset,
			       size_t count)
{
	memcpy((void *)dest_base + dest_offset, (void *)src_base + src_offset,
	       count);
}

/*
 * setup kernel memory range for 
 *     guest kernel
 *     guest payload
 */
int kkm_mmu_copy_kernel_pgd(struct kkm *kkm)
{
	/* when running in kernel mode we are expected to have kernel pgd */
	uint64_t current_pgd_base = (uint64_t)kkm->mm->pgd;

	if (current_pgd_base == 0) {
		printk(KERN_NOTICE
		       "kkm_mmu_copy_kernel_pgd: PGD base is zero\n");
		return -EINVAL;
	}

	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			   kkm->guest_kernel_va, KKM_PGD_KERNEL_OFFSET,
			   KKM_PGD_KERNEL_SIZE);

	/* set private area in kernel pml4 area */
	kkm_mmu_set_entry((void *)kkm->guest_kernel_va, KKM_PGD_INDEX,
			  kkm_mmu.pgd_entry);

	/*
	 * point to user pgd.
	 * linux kernel allocates kernel and user pml4 tables
	 * next to each other.
	 * kernel pml4 is even page
	 * user pml4 is odd page
	 */

	/* keep all memory map for now.
	 * current_pgd_base += PAGE_SIZE;
	 */
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			   kkm->guest_payload_va, KKM_PGD_KERNEL_OFFSET,
			   KKM_PGD_KERNEL_SIZE);

	/* set private area in guest pml4 */
	kkm_mmu_set_entry((void *)kkm->guest_payload_va, KKM_PGD_INDEX,
			  kkm_mmu.pgd_entry);

	return 0;
}

/*
 * setup guest payload area
 * memory from 16TB to 16TB + 512GB is mapped in monitor.
 * copy the above pml4 entry to point to 0TB in guest payload for text
 * copy the above pml4 entry to point to 128TB in guest payload for stack and mmap
 */
int kkm_mmu_sync(struct kkm *kkm)
{
	uint64_t current_pgd_base = (uint64_t)kkm->mm->pgd;
	uint64_t *pgd_pointer = NULL;

	/*
	 * keep kernel and user pgd same for payload area
	 * entry 0 for code + data
	 */
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			   kkm->guest_kernel_va, KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			   KKM_PGD_PAYLOAD_SIZE);
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			   kkm->guest_payload_va,
			   KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			   KKM_PGD_PAYLOAD_SIZE);

	/* entry 255 for stack + mmap */
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			   kkm->guest_kernel_va,
			   KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			   KKM_PGD_PAYLOAD_SIZE);
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			   kkm->guest_payload_va,
			   KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			   KKM_PGD_PAYLOAD_SIZE);

	/* change pml4 entry 0 to allow execution */
	pgd_pointer = (uint64_t *)kkm->guest_payload_va;
	if (pgd_pointer[0] & _PAGE_NX) {
		printk(KERN_NOTICE
		       "kkm_mmu_sync: entry 0 has execute disable set, enable it.\n");
		pgd_pointer[0] &= ~_PAGE_NX;
	}

	/*
	 * fix memory alias created
	 * modify km to use one pml4 entry for code + data and second entry for stack + mmap
	 */
	return 0;
}
