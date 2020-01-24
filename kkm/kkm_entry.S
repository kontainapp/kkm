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

.code64

.globl kkm_trap_entry
.align 16
kkm_trap_entry:
.size kkm_trap_entry, .-kkm_trap_entry

.global kkm_swith_to_guest
.align 16
kkm_swith_to_guest:
.size kkm_swith_to_guest, .-kkm_swith_to_guest
