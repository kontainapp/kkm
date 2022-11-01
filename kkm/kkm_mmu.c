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
#include <linux/log2.h>
#include <asm/io.h>
#include <asm/tlbflush.h>

#include "kkm.h"
#include "kkm_mm.h"

/*
 * page table hierarchy for kx area
 */
struct kkm_mmu_pml4e kkm_mmu_kx;

/*
 * return current cpu private area first page table index
 * next KKM_PER_CPU_GA_PAGE_COUNT belong to this physical cpu
 */
static int kkm_mmu_get_per_cpu_start_index(int cpu_index)
{
	int page_index =
		KKM_CPU_GA_INDEX_START + cpu_index * KKM_PER_CPU_GA_PAGE_COUNT;
	return page_index;
}

/*
 * set one page table entry
 */
static void kkm_mmu_set_entry(void *pt_va, int index, uint64_t entry)
{
	((uint64_t *)pt_va)[index] = entry;
}

/*
 * set one page table entry with given physical address and flags
 */
static void kkm_mmu_insert_page(void *pt_va, int index, phys_addr_t pa,
				uint64_t flags)
{
	((uint64_t *)pt_va)[index] =
		(pa & KKM_PAGE_PA_MASK) | (flags & KKM_PAGE_FLAGS_MASK);
}

static inline uint64_t kkm_mmu_entry_pa(uint64_t entry)
{
	return entry & PTE_PFN_MASK;
}

static bool kkm_mmu_get_table_va(uint64_t *table_va, int index)
{
	uint64_t entry;
	uint64_t entry_pa;
	uint64_t next_table_va;

	/*
	 * fetch entry from page table
	 */
	entry = ((uint64_t *)(*table_va))[index];

	/*
	 * get physical address of page from entry
	 */
	entry_pa = kkm_mmu_entry_pa(entry);

	/*
	 * convert physical address of page to kernel virtual address
	 */
	next_table_va = (uint64_t)phys_to_virt(entry_pa);

	if (entry & _PAGE_PRESENT) {
		*table_va = next_table_va;
		return true;
	}
	return false;
}

/*
 * allocate pages and initialize pud, pmd, pt for private area
 */
int kkm_mmu_init(void)
{
	return kkm_create_pml4(&kkm_mmu_kx, KKM_PRIVATE_START_VA);
}

void kkm_mmu_cleanup(void)
{
	kkm_mmu_flush_tlb();
	kkm_cleanup_pml4(&kkm_mmu_kx);
}

/*
 * flush TLB entries for GUEST_KERNEL_PCID and GUEST_PAYLOAD_PCID
 */
void kkm_mmu_flush_tlb(void)
{
	invpcid_flush_single_context(GUEST_KERNEL_PCID);
	invpcid_flush_single_context(GUEST_PAYLOAD_PCID);
}

/*
 * flush one page TLB entry for GUEST_KERNEL_PCID and GUEST_PAYLOAD_PCID
 */
void kkm_mmu_flush_tlb_one_page(uint64_t addr)
{
	invpcid_flush_one(GUEST_KERNEL_PCID, addr);
	invpcid_flush_one(GUEST_PAYLOAD_PCID, addr);
}

/*
 * allocate pages and initialize page table hierrarchy
 */
int kkm_create_pml4(struct kkm_mmu_pml4e *kmu, uint64_t address)
{
	int ret_val = 0;
	int pud_idx;
	int pmd_idx;

	memset(kmu, 0, sizeof(struct kkm_mmu_pml4e));

	/* alocate page for pud */
	ret_val = kkm_mm_allocate_page(&kmu->pud.page, &kmu->pud.va,
				       &kmu->pud.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_create_pml4: failed to allocate pud page error(%d)\n",
		       ret_val);
		goto error;
	}
	/* alocate page for pmd */
	ret_val = kkm_mm_allocate_page(&kmu->pmd.page, &kmu->pmd.va,
				       &kmu->pmd.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_create_pml4: failed to allocate pmd page error(%d)\n",
		       ret_val);
		goto error;
	}
	/* alocate page for pt */
	ret_val = kkm_mm_allocate_page(&kmu->pt.page, &kmu->pt.va, &kmu->pt.pa);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_create_pml4: failed to allocate pt page error(%d)\n",
		       ret_val);
		goto error;
	}

	/* pages are allocated and zeroed, __GFP_ZERO flag is used to allocate page */

	/* create pgd entry */
	kmu->pgd_entry = (kmu->pud.pa & KKM_PAGE_PA_MASK) | _PAGE_USER |
			 _PAGE_RW | _PAGE_PRESENT;

	/* initialize first entry in pud */
	pud_idx = pud_index(address);
	kkm_mmu_insert_page(kmu->pud.va, pud_idx, kmu->pmd.pa,
			    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

	/* initialize first entry in pmd */
	pmd_idx = pmd_index(address);
	kkm_mmu_insert_page(kmu->pmd.va, pmd_idx, kmu->pt.pa,
			    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

error:
	if (ret_val != 0) {
		kkm_mmu_cleanup();
	}
	return ret_val;
}

void kkm_cleanup_pml4(struct kkm_mmu_pml4e *kmu)
{
	if (kmu->pud.page != NULL) {
		kkm_mm_free_page(kmu->pud.va);
	}
	if (kmu->pmd.page != NULL) {
		kkm_mm_free_page(kmu->pmd.va);
	}
	if (kmu->pt.page != NULL) {
		kkm_mm_free_page(kmu->pt.va);
	}
}

/*
 * set this physicl cpu private area page table entries
 */
void kkm_mmu_set_guest_area(int cpu_index, phys_addr_t pa0, phys_addr_t pa1,
			    phys_addr_t pa2, phys_addr_t pa3)
{
	int page_index = kkm_mmu_get_per_cpu_start_index(cpu_index);
	uint64_t flags = _PAGE_NX | _PAGE_RW | _PAGE_PRESENT;

	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, page_index, pa0, flags);
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, page_index + 1, pa1, flags);
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, page_index + 2, pa2, flags);
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, page_index + 3, pa3, flags);
}

/*
 * return start virtual address of this physical cpu guest private address
 */
void *kkm_mmu_get_cur_cpu_guest_va(int cpu_index)
{
	int page_index = kkm_mmu_get_per_cpu_start_index(cpu_index);
	uint64_t va = KKM_PRIVATE_START_VA + page_index * PAGE_SIZE;

	return (void *)va;
}

void *kkm_mmu_get_idt_va(void)
{
	return (void *)KKM_PRIVATE_START_VA;
}

void kkm_mmu_set_kx_global_info(phys_addr_t idt_pa, phys_addr_t text_page0_pa,
				phys_addr_t text_page1_pa,
				phys_addr_t kx_global_pa)
{
	/*
	 * pte KKM_PTE_INDEX_IDT corresponds to KKM_IDT_START_VA
	 */
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, KKM_PTE_INDEX_IDT, idt_pa,
			    _PAGE_NX | _PAGE_PRESENT);

	/*
	 * pte KKM_PTE_INDEX_TEXT_0 corresponds to KKM_IDT_GLOBAL_START
	 */
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, KKM_PTE_INDEX_TEXT_0,
			    text_page0_pa, _PAGE_RW | _PAGE_PRESENT);
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, KKM_PTE_INDEX_TEXT_1,
			    text_page1_pa, _PAGE_RW | _PAGE_PRESENT);

	/*
	 * pte KKM_PTE_INDEX_KX corresponds to KKM_IDT_GLOBAL_START
	 */
	kkm_mmu_insert_page(kkm_mmu_kx.pt.va, KKM_PTE_INDEX_KX, kx_global_pa,
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

static void kkm_mmu_setup_one_pgd(uint64_t kernel_pgd_base,
				  void *pgd_table_base, void *p4d_table_base)
{
	uint64_t kernel_pgd_kx_p4d_entry_pa = 0;
	uint64_t kernel_pgd_kx_p4d_entry_va = 0;

	kkm_mmu_copy_range(kernel_pgd_base, KKM_PGD_KERNEL_OFFSET,
			   (uint64_t)pgd_table_base, KKM_PGD_KERNEL_OFFSET,
			   KKM_PGD_KERNEL_SIZE);

	if (pgtable_l5_enabled() == true) {
		if (p4d_table_base == NULL) {
			printk(KERN_NOTICE
			       "kkm_mmu_setup_one_pgd: P4D base is zero when pgtable_l5_enabled\n");
			return;
		}
		/*
		 * we have already copied top half of pgd from linux kernel to kkm pgd
		 * replace pgd entry corresponding to KKM_KX_PGD_P4D_INDEX(511) to our p4d entry
		 * copy entire p4d from native kernel to our p4d
		 * replace entry KKM_KX_PGD_INDEX with kx entry
		 */
		kernel_pgd_kx_p4d_entry_pa =
			((uint64_t *)kernel_pgd_base)[KKM_KX_PGD_P4D_INDEX] &
			KKM_PAGE_PA_MASK;
		kernel_pgd_kx_p4d_entry_va =
			(uint64_t)phys_to_virt(kernel_pgd_kx_p4d_entry_pa);
		kkm_mmu_copy_range(kernel_pgd_kx_p4d_entry_va, 0,
				   (uint64_t)p4d_table_base, 0, PAGE_SIZE);

		/*
		 * set kx private area in p4d
		 */
		kkm_mmu_set_entry(p4d_table_base, KKM_KX_PGD_INDEX,
				  kkm_mmu_kx.pgd_entry);

		/*
		 * set kx p4d entry in pgd
		 */
		kkm_mmu_insert_page(pgd_table_base, KKM_KX_PGD_P4D_INDEX,
				    virt_to_phys(p4d_table_base),
				    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);
	} else {
		/*
		 * set kx private area in pgd
		 */
		kkm_mmu_set_entry(pgd_table_base, KKM_KX_PGD_INDEX,
				  kkm_mmu_kx.pgd_entry);
	}
}

/*
 * setup kernel memory range for 
 *     guest kernel
 *     guest payload
 */
int kkm_mmu_copy_kernel_pgd(uint64_t current_pgd_base, void *guest_kernel_va,
			    void *guest_payload_va, void *kernel_p4d_va,
			    void *payload_p4d_va)
{
	if (current_pgd_base == 0) {
		printk(KERN_NOTICE
		       "kkm_mmu_copy_kernel_pgd: PGD base is zero\n");
		return -EINVAL;
	}

	kkm_mmu_setup_one_pgd(current_pgd_base, guest_kernel_va, kernel_p4d_va);

	/*
	 * point to user pgd.
	 * linux kernel allocates kernel and user pml4 tables
	 * next to each other.
	 * kernel pml4 is even page
	 * user pml4 is odd page
	 */
	//current_pgd_base += PAGE_SIZE;

	kkm_mmu_setup_one_pgd(current_pgd_base, guest_payload_va,
			      payload_p4d_va);

	return 0;
}

/*
 * setup guest payload area
 * memory from 16TB to 16TB + 512GB is mapped in monitor.
 * copy the above pml4 entry to point to 0TB in guest payload for text
 * copy the above pml4 entry to point to 128TB in guest payload for stack and mmap
 */
int kkm_mmu_sync(uint64_t current_pgd_base, void *guest_kernel_va,
		 void *guest_payload_va, struct kkm_mmu_pml4e *guest,
		 void *low_p4d_va, phys_addr_t low_p4d_pa)
{
	uint64_t native_kernel_entry = -1;
	uint64_t guest_payload_entry = -1;
	uint64_t new_guest_entry = -1;
	uint64_t native_kernel_low_p4d_pa = 0;
	uint64_t native_kernel_low_p4d_va = 0;

	if (pgtable_l5_enabled() == true) {
		/*
		 * from kernel pgd get p4d pointer(virtual address) for KKM_PGD_LOW_P4D_ENTRY
		 */

		native_kernel_low_p4d_pa =
			((uint64_t *)current_pgd_base)[KKM_PGD_LOW_P4D_ENTRY] &
			KKM_PAGE_PA_MASK;
		native_kernel_low_p4d_va =
			(uint64_t)phys_to_virt(native_kernel_low_p4d_pa);

		native_kernel_entry = ((uint64_t *)native_kernel_low_p4d_va)
			[KKM_PGD_MONITOR_PAYLOAD_ENTRY];
		guest_payload_entry =
			((uint64_t *)
				 low_p4d_va)[KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY];
	} else {
		native_kernel_entry =
			((uint64_t *)
				 current_pgd_base)[KKM_PGD_MONITOR_PAYLOAD_ENTRY];
		guest_payload_entry = ((uint64_t *)guest_payload_va)
			[KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY];
	}

	/*
	 * if entries Physical Address are same, nothing to be done
	 */
	if (kkm_mmu_entry_pa(native_kernel_entry) ==
	    kkm_mmu_entry_pa(guest_payload_entry)) {
		return 0;
	}

	/*
	 * change pml4 entry to allow execution
	 * km code is in entry 0 and mmap'ed libraries are in entry 255
	 */
	new_guest_entry = native_kernel_entry & ~_PAGE_NX;

	if (pgtable_l5_enabled() == true) {
		/*
		 * insert pgd entry using low_p4d_pa
		 * in guest_kernel and guest_payload pgd
		 */
		kkm_mmu_insert_page(guest_kernel_va, KKM_PGD_LOW_P4D_ENTRY,
				    low_p4d_pa,
				    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);
		kkm_mmu_insert_page(guest_payload_va, KKM_PGD_LOW_P4D_ENTRY,
				    low_p4d_pa,
				    _PAGE_USER | _PAGE_RW | _PAGE_PRESENT);

		/*
		 * keep kernel and user pgd same for payload area
		 * entry 0 for code + data
		 */
		((uint64_t *)low_p4d_va)[KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY] =
			new_guest_entry;

		/*
		 * set entry 1 for guest va(vdso, vvar + code copied from km to payload)
		 */
		((uint64_t *)low_p4d_va)[1] = guest->pgd_entry;

		/*
		 * entry 255 for stack + mmap
		 */
		((uint64_t *)low_p4d_va)[KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY] =
			new_guest_entry;
	} else {
		/*
		 * keep kernel and user pgd same for payload area
		 * entry 0 for code + data
		 */
		((uint64_t *)
			 guest_kernel_va)[KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY] =
			new_guest_entry;
		((uint64_t *)
			 guest_payload_va)[KKM_PGD_GUEST_PAYLOAD_BOTTOM_ENTRY] =
			new_guest_entry;

		/*
		 * set entry 1 for guest va(vdso, vvar + code copied from km to payload)
		 */
		((uint64_t *)guest_kernel_va)[1] = guest->pgd_entry;
		((uint64_t *)guest_payload_va)[1] = guest->pgd_entry;

		/*
		 * entry 255 for stack + mmap
		 */
		((uint64_t *)guest_kernel_va)[KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY] =
			new_guest_entry;
		((uint64_t *)guest_payload_va)[KKM_PGD_GUEST_PAYLOAD_TOP_ENTRY] =
			new_guest_entry;
	}

	/*
	 * fix memory alias created
	 * modify km to use one pml4 entry for code + data and second entry for stack + mmap
	 */
	return 0;
}

/*
 * walk through kernel page table to identify physical address of faulted address
 * add to to guest kernel and payload page tables
 */
bool kkm_mmu_update_priv_area(uint64_t guest_fault_address,
			      uint64_t monitor_fault_address,
			      uint64_t current_pgd_base,
			      struct kkm_mmu_pml4e *guest)
{
	bool ret_val = true;
	uint64_t pgd_idx = pgd_index(monitor_fault_address);
	uint64_t p4d_idx = p4d_index(monitor_fault_address);
	uint64_t pud_idx = pud_index(monitor_fault_address);
	uint64_t pmd_idx = pmd_index(monitor_fault_address);
	uint64_t pte_idx = pte_index(monitor_fault_address);
	uint64_t table_va = current_pgd_base;
	uint64_t gva_pte_idx = pte_index(guest_fault_address);

	/* top level */
	if (kkm_mmu_get_table_va(&table_va, pgd_idx) != true) {
		printk(KERN_NOTICE "kkm_mmu_update_priv_area: pgd failed\n");
		ret_val = false;
		goto end;
	}

	if (pgtable_l5_enabled() == true) {
		if (kkm_mmu_get_table_va(&table_va, p4d_idx) != true) {
			printk(KERN_NOTICE
			       "kkm_mmu_update_priv_area: p4d failed\n");
			ret_val = false;
			goto end;
		}
	}

	/* 3rd level */
	if (kkm_mmu_get_table_va(&table_va, pud_idx) != true) {
		printk(KERN_NOTICE "kkm_mmu_update_priv_area: pud failed\n");
		ret_val = false;
		goto end;
	}

	/* 2nd level */
	if (kkm_mmu_get_table_va(&table_va, pmd_idx) != true) {
		printk(KERN_NOTICE "kkm_mmu_update_priv_area: pmd failed\n");
		ret_val = false;
		goto end;
	}

	/* final paga table */
	((uint64_t *)guest->pt.va)[gva_pte_idx] =
		((uint64_t *)table_va)[pte_idx];

end:
	if (ret_val == false) {
		printk(KERN_NOTICE
		       "kkm_mmu_update_priv_area: page walk failed kernel table va %llx "
		       "guest address %llx pgd %llx p4d %llx pud %llx pmd %llx pte %llx\n",
		       table_va, monitor_fault_address, pgd_idx, p4d_idx,
		       pud_idx, pmd_idx, pte_idx);
	}
	return ret_val;
}
