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

#ifndef __KKM_FPU_H__
#define __KKM_FPU_H__

void kkm_fpu_save_xstate_xsaves(void *xstate_buf);
void kkm_fpu_restore_xstate_xsaves(void *xstate_buf);
void kkm_fpu_save_xstate_xsave(void *xstate_buf);
void kkm_fpu_restore_xstate_xsave(void *xstate_buf);

#endif /* __KKM_FPU_H__ */
