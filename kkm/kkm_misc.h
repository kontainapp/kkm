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

#ifndef __KKM_MISC_H__
#define __KKM_MISC_H__

void kkm_idt_invalidate(void *address);
void kkm_flush_tlb_all(void);
void kkm_change_address_space(phys_addr_t pgd_pa);

#endif /* __KKM_MISC_H__ */
