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

#ifndef __KKM_IDT_CACHE_H__
#define __KKM_IDT_CACHE_H__

int kkm_idt_cache_init(void);
void kkm_idt_cache_cleanup(void);
int kkm_idt_get_desc(struct desc_ptr *native_desc, struct desc_ptr *guest_desc);
void kkm_idt_set_id(int cpu, uint64_t id);
uint64_t kkm_idt_get_id(int cpu);

#endif /* __KKM_IDT_CACHE_H__ */
