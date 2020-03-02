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

#ifndef __KKM_IOCTL_H__
#define __KKM_IOCTL_H__

#include <linux/types.h>
#include <linux/ioctl.h>

/* various ioctl's and their respective data structures */

/* device base name */
#define KKM_DEVICE_NAME "kkm"

/* base ioctl type */
#define KKM_IO (0xAE)

#define KKM_GET_VERSION _IO(KKM_IO, 0)
#define KKM_CREATE_KONTAINER _IO(KKM_IO, 1)
#define KKM_GET_CONTEXT_MAP_SIZE _IO(KKM_IO, 4)
#define KKM_GET_SUPPORTED_CONTEXT_INFO _IOWR(KKM_IO, 5, struct kkm_cpuid)

#define KKM_ADD_EXECUTION_CONTEXT _IO(KKM_IO, 0x41)
#define KKM_MEMORY _IOW(KKM_IO, 0x46, struct kkm_memory_region)
#define KKM_SET_ID_MAP_ADDR _IOW(KKM_IO, 0x48, uint64_t)

#define KKM_RUN _IO(KKM_IO, 0x80)
#define KKM_GET_REGS _IOR(KKM_IO, 0x81, struct kkm_regs)
#define KKM_SET_REGS _IOW(KKM_IO, 0x82, struct kkm_regs)
#define KKM_GET_SREGS _IOR(KKM_IO, 0x83, struct kkm_sregs)
#define KKM_SET_SREGS _IOW(KKM_IO, 0x84, struct kkm_sregs)

#define KKM_GET_FPU _IOR(KKM_IO, 0x8c, struct kkm_fpu)
#define KKM_SET_FPU _IOW(KKM_IO, 0x8d, struct kkm_fpu)

#define KKM_SET_CPUID _IOW(KKM_IO, 0x90, struct kkm_cpuid)

#define KKM_SET_DEBUG _IOW(KKM_IO, 0x9b, struct kkm_debug)
#define KKM_GET_EVENTS _IOR(KKM_IO, 0x9f, struct kkm_ec_events)

// KKM_GET_SUPPORTED_CONTEXT_INFO
#define KKM_CONTEXT_INFO_ENTRY_COUNT (6)
struct kkm_ec_entry {
	uint32_t function;
	uint32_t index;
	uint32_t flags;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t reserved[3];
};

struct kkm_cpuid {
	uint32_t entry_count;
	uint32_t reserved;
	struct kkm_ec_entry entries[0];
};

// KKM_GET_REGS & KKM_SET_REGS
struct kkm_regs {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;

	uint64_t rsi;
	uint64_t rdi;
	uint64_t rsp;
	uint64_t rbp;

	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;

	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint64_t rip;
	uint64_t rflags;
};

// KKM_GET_SREGS & KKM_SET_SREGS
#define KKM_INTERRUPT_COUNT (256)
#define KKM_INTERRUPT_BITMAP_SIZE ((KKM_INTERRUPT_COUNT + 63) / 64)

struct kkm_segment {
	uint64_t base;
	uint32_t limit;
	uint16_t selector;
	uint8_t type;
	uint8_t present;
	uint8_t dpl;
	uint8_t db;
	uint8_t s;
	uint8_t l;
	uint8_t g;
	uint8_t avl;
	uint8_t unusable;
	uint8_t reserved;
};

struct kkm_dtable {
	uint64_t base;
	uint16_t limit;
	uint16_t reserved[3];
};

struct kkm_sregs {
	struct kkm_segment cs;
	struct kkm_segment ds;
	struct kkm_segment es;
	struct kkm_segment fs;
	struct kkm_segment gs;
	struct kkm_segment ss;

	struct kkm_segment tr;
	struct kkm_segment ldt;

	struct kkm_dtable gdt;
	struct kkm_dtable idt;

	uint64_t cr0;
	uint64_t cr2;
	uint64_t cr3;
	uint64_t cr4;
	uint64_t cr8;

	uint64_t efer;
	uint64_t apic_base;
	uint64_t intr_bitmap[KKM_INTERRUPT_BITMAP_SIZE];
};

// KKM_GET_FPU & KKM_SET_FPU
struct kkm_fpu {
	uint64_t reserved;
};

// KKM_SET_DEBUG
struct kkm_debug {
	uint32_t control;
	uint32_t reserved;
	uint64_t registers[8];
};

// KKM_GET_EVENTS
struct kkm_ec_events {
	struct {
		uint8_t injected;
		uint8_t nr;
		uint8_t has_error_code;
		uint8_t pending;
		uint32_t error_code;
	} exception;
	struct {
		uint8_t interrupt;
		uint8_t nr;
		uint8_t soft;
		uint8_t shadow;
	} interrupt;
	struct {
		uint8_t injected;
		uint8_t pending;
		uint8_t masked;
		uint8_t reserved_nmi;
	} nmi;
	uint32_t sipi_vector;
	uint32_t flags;
	struct {
		uint8_t smm;
		uint8_t pending;
		uint8_t smm_inside_nmi;
		uint8_t latched_init;
	} smi;
	uint8_t reserved[27];
	uint8_t excepption_has_payload;
	uint64_t exception_payload;
};

// KKM_APP_MEMORY
struct kkm_memory_region {
	uint32_t slot;
	uint32_t flags;
	uint64_t guest_phys_addr;
	uint64_t memory_size;
	uint64_t userspace_addr;
};

#define KKM_MAX_CONTEXTS (64)
#define KKM_MAX_MEMORY_SLOTS (64)

#endif /* __KKM_IOCTL_H__ */
