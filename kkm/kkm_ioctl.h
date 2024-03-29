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

#ifndef __KKM_IOCTL_H__
#define __KKM_IOCTL_H__

#include <linux/types.h>
#include <linux/ioctl.h>

/* various ioctl's and their respective data structures */

/* device base name */
#define KKM_DEVICE_NAME "kkm"
#define KKM_DEVICE_IDENTITY (0x6B6B6D)

/* base ioctl type */
#define KKM_IO (0xAE)

#define KKM_GET_VERSION _IO(KKM_IO, 0)
#define KKM_CREATE_KONTAINER _IO(KKM_IO, 1)
#define KKM_CHECK_EXTENSION _IO(KKM_IO, 3)
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

#define KKM_SET_MSRS _IOW(KKM_IO, 0x89, struct kkm_msrs)

#define KKM_GET_FPU _IOR(KKM_IO, 0x8c, struct kkm_fpu)
#define KKM_SET_FPU _IOW(KKM_IO, 0x8d, struct kkm_fpu)

#define KKM_SET_CPUID _IOW(KKM_IO, 0x90, struct kkm_cpuid)

#define KKM_SET_DEBUG _IOW(KKM_IO, 0x9b, struct kkm_debug)
#define KKM_GET_EVENTS _IOR(KKM_IO, 0x9f, struct kkm_ec_events)

#define KKM_GET_XSAVE _IOR(KKM_IO, 0xa4, struct kkm_xsave)
#define KKM_SET_XSAVE _IOW(KKM_IO, 0xa5, struct kkm_xsave)

#define KKM_GET_XCRS _IOR(KKM_IO, 0xa6, struct kkm_xcrs)
#define KKM_SET_XCRS _IOW(KKM_IO, 0xa7, struct kkm_xcrs)

#define KKM_KONTEXT_REUSE _IO(KKM_IO, 0xf5)
#define KKM_KONTEXT_GET_SAVE_INFO _IOR(KKM_IO, 0xf6, struct kkm_save_info)
#define KKM_KONTEXT_SET_SAVE_INFO _IOW(KKM_IO, 0xf7, struct kkm_save_info)
#define KKM_KONTEXT_GET_XSTATE _IOR(KKM_IO, 0xf8, struct kkm_xstate)
#define KKM_KONTEXT_SET_XSTATE _IOW(KKM_IO, 0xf9, struct kkm_xstate)

#define KKM_CPU_SUPPORTED _IO(KKM_IO, 0xfe)
#define KKM_GET_IDENTITY _IO(KKM_IO, 0xff)

// capability check. values for KKM_CHECK_EXTENSION
#define KKM_CAP_SYNC_REGS (74)

// capabilites for KKM_CAP_SYNC_REGS
#define KKM_SYNC_X86_REGS (1ULL)
#define KKM_SYNC_X86_SREGS (2ULL)
#define KKM_SYNC_X86_EVENTS (4ULL)

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

struct kkm_msr_entry {
	uint32_t index;
	uint32_t reserved;
	uint64_t data;
};

struct kkm_msrs {
	uint32_t nmsrs;
	uint32_t pad;

	struct kkm_msr_entry entries[0];
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
	uint8_t fpr[8][16];
	uint16_t fcw;
	uint16_t fsw;
	uint8_t ftwx;
	uint8_t pad1;
	uint16_t last_opcode;
	uint64_t last_ip;
	uint64_t last_dp;
	uint8_t xmm[16][16];
	uint32_t mxcsr;
	uint32_t pad2;
};

// kkm_debug control flags
#define KKM_GUESTDBG_ENABLE (0x1)
#define KKM_GUESTDBG_SINGLESTEP (0x2)

#define KKM_GUESTDBG_USE_SW_BP (0x10000)
#define KKM_GUESTDBG_USE_HW_BP (0x20000)
#define KKM_GUESTDBG_INJECT_DB (0x40000)
#define KKM_GUESTDBG_INJECT_BP (0x80000)

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
	uint8_t exception_has_payload;
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

#define KKM_MAX_CONTEXTS (288)
#define KKM_MAX_MEMORY_SLOTS (64)

enum fault_reason {
	FAULT_UNKNOWN = 0,
	FAULT_HYPER_CALL = 1,
	FAULT_SYSCALL = 2,
};

struct kkm_private_area {
	uint32_t data;
	enum fault_reason reason;
};
static_assert(sizeof(struct kkm_private_area) == 8,
	      "kkm_private_area is known to monitor, size is fixed at 8 bytes");

/*
 * XSAVE get and set
 * KKM_GET_XSAVE and KKM_SET_XSAVE
 */
struct kkm_xsave {
	uint8_t regions[4096];
};
static_assert(sizeof(struct kkm_xsave) == 4096,
	      "kkm_xsave is known to monitor, size is fixed at 4096 bytes");

/*
 * XCR get and set
 * KKM_GET_XCRS and KKM_SET_XCRS
 */
#define KKM_MAX_XCRS (16)

struct kkm_xcr {
	uint32_t xcr;
	uint32_t padding;
	uint64_t value;
};
static_assert(sizeof(struct kkm_xcr) == 16,
	      "kkm_xcr is known to monitor, size is fixed at 16 bytes");

struct kkm_xcrs {
	uint32_t nr_xcrs;
	uint32_t flags;
	struct kkm_xcr xcrs[KKM_MAX_XCRS];
	uint64_t padding[16];
};
static_assert(sizeof(struct kkm_xcrs) == 392,
	      "kkm_xcrs is known to monitor, size is fixed at 392 bytes");

#define KKM_SAVE_INFO_SZ (64)
/*
 * signal handling support
 * KKM_KONTEXT_GET_SAVE_INFO and KKM_KONTEXT_SET_SAVE_INFO
 */
struct kkm_save_info {
	union {
		struct {
			bool syscall_pending;
			uint64_t ret_val_mva;
			bool exception_posted;
			bool first_thread;
			bool new_thread;
			uint64_t exception_saved_rax;
			uint64_t exception_saved_rbx;
			uint32_t hypercall_data;
			enum fault_reason reason;
		};
		uint8_t data[KKM_SAVE_INFO_SZ];
	};
};
static_assert(sizeof(struct kkm_save_info) == 64,
	      "kkm_save_info is known to monitor, size is fixed at 64 bytes");

typedef enum {
	KKM_NONE = 0,
	KKM_XSAVE = 1,
	KKM_XSAVES = 2,
} kkm_xstate_format_t;
static_assert(sizeof(kkm_xstate_format_t) == 4,
	      "kkm_xstate_format_t size is fixed at 4 bytes");

/*
 * PAGE_SIZE - xstate valid - save format type - crc
 */
#define KKM_XSTATE_DATA_SIZE (4084)
/*
 * signal handling support
 * KKM_KONTEXT_GET_XSTATE and KKM_KONTEXT_SET_XSTATE
 */
struct kkm_xstate {
	uint8_t data[KKM_XSTATE_DATA_SIZE];
	union {
		bool valid;
		uint32_t padding;
	};
	kkm_xstate_format_t format;
	uint32_t crc32;
};
static_assert(sizeof(struct kkm_xstate) == 4096,
	      "kkm_xstate is known to monitor, size is fixed at 4096 bytes");

/*
 * KKM_CPU_SUPPORTED
 */
enum kkm_cpu_supported { CPU_SUPPORTED = 0, CPU_NOT_SUPPORTED };

#endif /* __KKM_IOCTL_H__ */
