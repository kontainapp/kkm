// SPDX-License-Identifier: GPL-2.0
/*
 * Kontain Kernel Module
 *
 * This module enables Kontain unikernel in absence of
 * hardware support for virtualization
 *
 * Copyright (C) 2023-2023 Kontain Inc.
 *
 * Authors:
 *  Srinivasa Vetsa <svetsa@kontain.app>
 *
 */

#include <linux/linkage.h>
#include <asm/percpu.h>
#include <asm/asm-offsets.h>
#include "kkm_offsets.h"

	.text
	.align 4096

	.global kkm_aife_error
kkm_aife_error:
	endbr64
	retq
	int3
	.size kkm_aife_error, .-kkm_aife_error

	.align 16
	.global kkm_aife
kkm_aife:
	endbr64
	retq
	int3
	.size kkm_aife, .-kkm_aife

