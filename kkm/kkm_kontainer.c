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
#include <linux/interrupt.h>
#include <asm/traps.h>
#include <asm/desc.h>

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_kontext.h"
#include "kkm_kontainer.h"
#include "kkm_mm.h"
#include "kkm_guest_entry.h"

/*
 * PTI_USER_PGTABLE_MASK not visible to modules
 * make up our own mask
 */
#define KKM_USER_PGTABLE_BIT (PAGE_SHIFT)
#define KKM_USER_PGTABLE_MASK (1 << KKM_USER_PGTABLE_BIT)

#define KKM_KONTAINER_LVL_PAGE_COUNT (2)
#define KKM_KONTAINER_LOW_PAGE_COUNT (1)

int kkm_kontainer_allocate_lvl_pages(struct kkm_mmu_page_info *k_page,
				     struct kkm_mmu_page_info *p_page)
{
	int ret_val = 0;

	/*
	 * allocate 2 pages
	 */
	ret_val = kkm_mm_allocate_pages(&k_page->page, &k_page->va, &k_page->pa,
					KKM_KONTAINER_LVL_PAGE_COUNT);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_allocate_lvl_pages: Failed to allocate memory for guest kernel page table error(%d)\n",
		       ret_val);
		return ret_val;
	}

	if (((uint64_t)k_page->va & KKM_USER_PGTABLE_MASK) ==
	    KKM_USER_PGTABLE_MASK) {
		printk(KERN_ERR
		       "kkm_kontainer_allocate_lvl_pages: unexpected odd start page address\n");
		ret_val = -EINVAL;
		return ret_val;
	}

	/*
	 * allocate 2 pages,
	 *     kernel mode
	 *     guest mode
	 * kernel code depends on kernel page table at even page and user page table at next(odd) page
	 */
	p_page->va = k_page->va + PAGE_SIZE;
	p_page->pa = k_page->pa + PAGE_SIZE;

	return ret_val;
}

int kkm_kontainer_allocate_pgd_pages(struct kkm *kkm)
{
	return kkm_kontainer_allocate_lvl_pages(&kkm->gk_pgd, &kkm->gp_pgd);
}

int kkm_kontainer_allocate_p4d_pages(struct kkm *kkm)
{
	return kkm_kontainer_allocate_lvl_pages(&kkm->gk_p4d, &kkm->gp_p4d);
}

int kkm_kontainer_init(struct kkm *kkm)
{
	int ret_val = 0;

	ret_val = kkm_kontainer_allocate_pgd_pages(kkm);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_init: pgd page allocation failed error(%d)\n",
		       ret_val);
		goto error;
	}

	if (pgtable_l5_enabled() == true) {
		ret_val = kkm_kontainer_allocate_p4d_pages(kkm);
		if (ret_val != 0) {
			printk(KERN_NOTICE
			       "kkm_kontainer_init: p4d page allocation failed error(%d)\n",
			       ret_val);
			goto error;
		}

		/*
		 * allocate low address page
		 */
		ret_val = kkm_mm_allocate_pages(&kkm->low_p4d.page,
						&kkm->low_p4d.va,
						&kkm->low_p4d.pa,
						KKM_KONTAINER_LOW_PAGE_COUNT);
		if (ret_val != 0) {
			printk(KERN_NOTICE
			       "kkm_kontainer_init: Failed to allocate memory for guest low pml4 page(%d)\n",
			       ret_val);
			return ret_val;
		}
	}

	ret_val = kkm_create_pml4(&kkm->kkm_guest_pml4e,
				  KKM_KM_GUEST_PRIVATE_MEM_START_VA);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_kontainer_init: guest area copy kernel pgd entry failed error(%d)\n",
		       ret_val);
		goto error;
	}

	mutex_init(&kkm->pf_lock);
	mutex_init(&kkm->mem_lock);
	mutex_init(&kkm->kontext_lock);

error:
	if (ret_val != 0) {
		kkm_kontainer_cleanup(kkm);
	}
	return ret_val;
}

void kkm_kontainer_cleanup_lvl_pages(struct kkm_mmu_page_info *k_page,
				     struct kkm_mmu_page_info *p_page)
{
	if (k_page->page != NULL) {
		kkm_mm_free_pages(k_page->va, KKM_KONTAINER_LVL_PAGE_COUNT);
		k_page->page = NULL;

		k_page->va = 0;
		k_page->pa = 0;

		p_page->va = 0;
		p_page->pa = 0;
	}
}

void kkm_kontainer_cleanup_pgd_pages(struct kkm *kkm)
{
	kkm_kontainer_cleanup_lvl_pages(&kkm->gk_pgd, &kkm->gp_pgd);
}

void kkm_kontainer_cleanup_p4d_pages(struct kkm *kkm)
{
	kkm_kontainer_cleanup_lvl_pages(&kkm->gk_p4d, &kkm->gp_p4d);
}

/*
 * cleanup
 */
void kkm_kontainer_cleanup(struct kkm *kkm)
{
	kkm_cleanup_pml4(&kkm->kkm_guest_pml4e);
	kkm_kontainer_cleanup_pgd_pages(kkm);
	kkm_kontainer_cleanup_p4d_pages(kkm);
}
