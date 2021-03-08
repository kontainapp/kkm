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

#ifndef __KKM_KONTAINER_H__
#define __KKM_KONTAINER_H__

int kkm_kontainer_init(struct kkm *kkm);
void kkm_kontainer_cleanup(struct kkm *kkm);

#endif /* __KKM_KONTAINER_H__ */
