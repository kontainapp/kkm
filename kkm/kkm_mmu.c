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


int kkm_mmu_init(void)
{
	int ret_val = 0;

	memset(&kkm_mmu, 0, sizeof(struct kkm_mmu));

	ret_val = kkm_mm_allocate_page(&kkm_mmu.pud.page, &kkm_mmu.pud.va, &kkm_mmu.pud.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE "kkm_mmu_init: failed to allocate pud page error(%d)\n", ret_val);
		goto error;
	}
	ret_val = kkm_mm_allocate_page(&kkm_mmu.pmd.page, &kkm_mmu.pmd.va, &kkm_mmu.pmd.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE "kkm_mmu_init: failed to allocate pmd page error(%d)\n", ret_val);
		goto error;
	}
	ret_val = kkm_mm_allocate_page(&kkm_mmu.pt.page, &kkm_mmu.pt.va, &kkm_mmu.pt.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE "kkm_mmu_init: failed to allocate pt page error(%d)\n", ret_val);
		goto error;
	}

	// setup page table hierarchy
	kkm_mmu.pgd_entry = (kkm_mmu.pud.pa & KKM_PAGE_PA_MASK) | _PAGE_USER | _PAGE_RW | _PAGE_PRESENT;
	kkm_mmu_insert_page(kkm_mmu.pud.va, 0, kkm_mmu.pmd.pa, _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu.pmd.va, 0, kkm_mmu.pt.pa, _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

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

uint64_t kkm_mmu_get_pgd_entry(void)
{
	return kkm_mmu.pgd_entry;
}

int kkm_mmu_get_per_cpu_start_index(void)
{
	int cpu = get_cpu();
	int page_index = KKM_CPU_GA_INDEX_START + cpu * KKM_PER_CPU_GA_PAGE_COUNT;
	return page_index;
}

void kkm_mmu_set_entry(void *pt_va, int index, uint64_t entry)
{
	((uint64_t *)pt_va)[index] = entry;
}

void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa, uint64_t flags)
{
	printk(KERN_NOTICE "kkm_mmu_insert_page: pt va %lx index %x pa %llx flags %llx\n",
		       (unsigned long)pt_va, index, pa, flags);
	((uint64_t *)pt_va)[index] = (pa & KKM_PAGE_PA_MASK) | (flags & KKM_PAGE_FLAGS_MASK);
}

void kkm_mmu_set_guest_area(phys_addr_t pa0, phys_addr_t pa1, phys_addr_t pa2, phys_addr_t pa3)
{
	int page_index = kkm_mmu_get_per_cpu_start_index();

	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index, pa0, _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 1, pa1, _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 2, pa2, _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu.pt.va, page_index + 3, pa3, _PAGE_RW | _PAGE_PRESENT);
}

void *kkm_mmu_get_cur_cpu_guest_va(void)
{
	int page_index = kkm_mmu_get_per_cpu_start_index();
	unsigned long long va = KKM_PRIVATE_START_VA + page_index * PAGE_SIZE;

	return (void *)va;
}

void kkm_mmu_set_idt(void *idt_va)
{
	phys_addr_t idt_pa;
	int cpu;

	cpu = get_cpu();
	idt_pa = virt_to_phys(idt_va);
	kkm_mmu_insert_page(kkm_mmu.pt.va, cpu, idt_pa, _PAGE_PRESENT);
}

void *kkm_mmu_get_idt_va(void)
{
	int cpu;

	cpu = get_cpu();
	return (void *)(KKM_PRIVATE_START_VA + cpu * PAGE_SIZE);
}

static void kkm_mmu_copy_range(unsigned long long src_base,
			      unsigned long long src_offset,
			      unsigned long long dest_base,
			      unsigned long long dest_offset, size_t count)
{
	memcpy((void *)dest_base + dest_offset, (void *)src_base + src_offset,
	       count);
}

int kkm_mmu_copy_kernel_pgd(struct kkm *kkm)
{
	// when running in kernel mode we are expected to have kernel pgd
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;

	if (current_pgd_base == 0) {
		printk(KERN_NOTICE "kkm_mmu_copy_kernel_pgd: PGD base is zero\n");
		return -EINVAL;
	}

	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_kernel_va, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);

	kkm_mmu_set_entry((void *)kkm->guest_kernel_va, KKM_PGD_INDEX, kkm_mmu.pgd_entry);

	// point to user pgd.
	// keep all memory map for now.
	// current_pgd_base += PAGE_SIZE;
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_KERNEL_OFFSET,
			  kkm->guest_payload_va, KKM_PGD_KERNEL_OFFSET,
			  KKM_PGD_KERNEL_SIZE);

	kkm_mmu_set_entry((void *)kkm->guest_payload_va, KKM_PGD_INDEX, kkm_mmu.pgd_entry);

	return 0;
}

int kkm_mmu_sync(struct kkm *kkm)
{
	unsigned long current_pgd_base = (unsigned long long)kkm->mm->pgd;
	unsigned long long *pgd_pointer = NULL;

	// keep kernel and user pgd same for payload area
	// entry 0 for code+data
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_kernel_va, KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			  KKM_PGD_PAYLOAD_SIZE);
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_payload_va, KKM_PGD_GUEST_PAYLOAD_OFFSET_0,
			  KKM_PGD_PAYLOAD_SIZE);

	// entry 255 for stack+mmap
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_kernel_va, KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			  KKM_PGD_PAYLOAD_SIZE);
	kkm_mmu_copy_range(current_pgd_base, KKM_PGD_MONITOR_PAYLOAD_OFFSET,
			  kkm->guest_payload_va, KKM_PGD_GUEST_PAYLOAD_OFFSET_255,
			  KKM_PGD_PAYLOAD_SIZE);

	// change pml4 entry 0 to allow execution
	pgd_pointer = (unsigned long long *)kkm->guest_payload_va;
	if (pgd_pointer[0] & _PAGE_NX) {
		printk(KERN_NOTICE "kkm_mmu_sync: entry 0 has execute disable set, enable it.\n");
		pgd_pointer[0] &= ~_PAGE_NX;
	}

	// fix memory alias created
	// modify km to use one pml4 entry for code + data and second entry for stack + mmap
	return 0;
}
