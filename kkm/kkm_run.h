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

#ifndef __KKM_RUN_H__
#define __KKM_RUN_H__

#define KKM_EXIT_IO_IN (0)
#define KKM_EXIT_IO_OUT (1)

enum kkm_exit_status {
	KKM_EXIT_UNKNOWN = 0,
	KKM_EXIT_EXCEPTION = 1,
	KKM_EXIT_IO = 2,
	KKM_EXIT_HYPERCALL = 3,
	KKM_EXIT_DEBUG = 4,
	KKM_EXIT_HLT = 5,
	KKM_EXIT_MMIO = 6,
	KKM_EXIT_IRQ_WINDOW_OPEN = 7,
	KKM_EXIT_SHUTDOWN = 8,
	KKM_EXIT_FAIL_ENTRY = 9,
	KKM_EXIT_INTR = 10,
	KKM_EXIT_NMI = 16,
	KKM_EXIT_INTERNAL_ERROR = 17,
	KKM_EXIT_OSI = 18,
	KKM_EXIT_PAPR_CALL = 19,
	KKM_EXIT_WATCHDOG = 21,
	KKM_EXIT_EPR = 23,
	KKM_EXIT_SYSTEM_EVENT = 24,
	KKM_EXIT_IOAPIC_EOI = 26,
};

struct kkm_debug_exit_arch {
	uint32_t exception;
	uint32_t reserved;
	uint64_t pc;
	uint64_t dr6;
	uint64_t dr7;
};

struct kkm_sync_regs {
	struct kkm_regs regs;
	struct kkm_sregs sregs;
	struct kkm_ec_events events;
};

struct kkm_run {
	uint8_t request_interrupt_window;
	uint8_t immediate_exit;
	uint8_t reserved[6];

	uint32_t exit_reason;
	uint8_t ready_for_interrupt_injection;
	uint8_t if_flag;
	uint16_t flags;

	uint64_t cr8;
	uint64_t apic_base;

	union {
		// KKM_EXIT_UNKNOWN
		struct {
			uint64_t hardware_exit_reason;
		} hw;
		// KKM_EXIT_FAIL_ENTRY
		struct {
			uint64_t hardware_entry_failure_reason;
		} fail_entry;
		// KKM_EXIT_EXCEPTION
		struct {
			uint32_t exception;
			uint32_t error_code;
		} ex;
		// KKM_EXIT_IO
		struct {
			uint8_t direction;
			uint8_t size;
			uint16_t port;
			uint32_t count;
			uint64_t data_offset;
		} io;
		// KKM_EXIT_DEBUG
		struct {
			struct kkm_debug_exit_arch arch;
		} debug;
		// KKM_EXIT_MMIO
		struct {
			uint64_t phys_addr;
			uint8_t data[8];
			uint32_t len;
			uint8_t is_write;
		} mmio;
		// KKM_EXIT_HYPERCALL
		struct {
			uint64_t nr;
			uint64_t args[6];
			uint64_t ret;
			uint32_t longmode;
			uint32_t pad;
		} hypercall;
		// KKM_EXIT_OSI
		struct {
			uint64_t gprs[32];
		} osi;
		// KKM_EXIT_PAPR_CALL
		struct {
			uint64_t nr;
			uint64_t ret;
			uint64_t args[9];
		} papr_hcall;
		// KKM_EXIT_EPR
		struct {
			uint32_t epr;
		} epr;
		// KKM_EXIT_SYSTEM_EVENT
		struct {
#define KKM_SYSTEM_EVENT_SHUTDOWN 1
#define KKM_SYSTEM_EVENT_RESET 2
#define KKM_SYSTEM_EVENT_CRASH 3
			uint32_t type;
			uint64_t flags;
		} system_event;
		// KKM_EXIT_IOAPIC_EOI
		struct {
			uint8_t vector;
		} eoi;
		int8_t reserved1[256];
	};

#define REGISTER_SIZE_BYTES 2048
	uint64_t kkm_valid_regs;
	uint64_t kkm_dirty_regs;
	union {
		struct kkm_sync_regs regs;
		int8_t reserved2[REGISTER_SIZE_BYTES];
	} s;
};

#endif /* __KKM_RUN_H__ */
