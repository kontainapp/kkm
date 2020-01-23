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

#ifndef __KKM_GUEST_H__
#define __KKM_GUEST_H__

int kkm_guest_switch_kernel(struct kkm_kontext *kkm_kontext);
int kkm_guest_init(struct kkm *kkm);
void kkm_guest_cleanup(struct kkm *kkm);


#endif /* __KKM_GUEST_H__ */
