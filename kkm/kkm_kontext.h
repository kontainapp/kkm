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

#ifndef __KKM_KONTEXT_H__
#define __KKM_KONTEXT_H__

/* per vcpu guest private area 8192 bytes */
#define KKM_GUEST_AREA_PAGES (2)
#define KKM_GUEST_AREA_SIZE (KKM_GUEST_AREA_PAGES * PAGE_SIZE)

/* guest private area used for private data */
#define GUEST_PRIVATE_DATA_SIZE (4096)
#define GUEST_STACK_REDZONE_SIZE (256)
#define GUEST_STACK_SIZE                                                       \
	(KKM_GUEST_AREA_SIZE - GUEST_PRIVATE_DATA_SIZE -                       \
	 GUEST_STACK_REDZONE_SIZE * 2)

#define GUEST_STACK_START_ADDRESS(guest_area_start)                            \
	(guest_area_start + ((struct kkm_guest_area *)0)->redzone)

#define REDZONE_DATA (0xa5)

#define KKM_GUEST_COPY_BUFFER (128)

#define KKM_KONTEXT_FAULT_PROCESS_DONE (256)
#define KKM_OUT_OPCODE (0xEF)
#define KKM_INTR_SYSCALL (511) /* system call instruction is executed */

/*
 * keep in sync with km_hcalls.h:km_hc_args
 */
struct kkm_intr_stack_no_error_code {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rdx;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

/*
 * keep in sync with km_hcalls.h:km_hc_args
 */
struct kkm_intr_stack_with_error_code {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rdx;
	uint64_t error;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

/*
 * keep in sync with km_hcalls.h:km_hc_args
 */
struct kkm_hc_args {
	uint64_t ret_val;
	uint64_t argument1;
	uint64_t argument2;
	uint64_t argument3;
	uint64_t argument4;
	uint64_t argument5;
	uint64_t argument6;
};

/*
 * trap info. HW pushes the following information on stack
 * before calling intr entry point
 */
struct kkm_trap_info {
	uint64_t error; /* optional error code */
	uint64_t rip; /* instruction pointer */
	uint64_t cs; /* code segment */
	uint64_t rflags; /* eflags register */
	uint64_t rsp; /* stack pointer */
	uint64_t ss; /* stack segment */
};

/*
 * per vcpu guest private area,
 * used for switching in and out of kontext payload
 * this will be mapped to per cpu private area just before switching to payload
 * and will be used as anchor for returning back to native kernel
 * minimal set of required data is stored in this structure
 * kontext ioctls copy to and from guest area to user space
 * top 4k is used as guest scratch pad and iret stack to payload
 * bottom 4k is used for stack with top and bottom redzone
 *
 * kkm_offsets.h maintains offsets of important members to be used in assembly code
 */
struct kkm_guest_area {
	// data store
	union {
		struct {
			struct kkm_kontext *kkm_kontext;
			uint64_t guest_area_beg; /* virtual address of this struct */
			uint64_t native_kernel_stack; /* %rsp before switching stacks */
			uint64_t guest_kernel_cr3; /* guest kernels pml4 pointer */
			uint64_t guest_kernel_cr4; /* guest kernel cr4 */
			uint64_t guest_payload_cr3; /* guest payload pml4 pointer */
			uint64_t reserved0; /* unused variable */

			/*
			 * we use our own cs and ss instead of km provided values.
			 * cs and ss have to be in sync with the way linux kernel gdt
			 */
			uint64_t guest_payload_cs; /* guest payload cs register */
			uint64_t guest_payload_ss; /* guest payload ss register value */

			/* native kernel idt */
			union {
				struct desc_ptr native_idt_desc;
				uint64_t native[2];
			};
			/* guest kernel idt */
			union {
				struct desc_ptr guest_idt_desc;
				uint64_t guest[2];
			};

			uint64_t native_save_tss_sp0;
			uint64_t native_save_tss_sp1;
			uint64_t native_save_tss_sp2;

			/*
			 * km sets the following members using ioctls
			 * use these to setup guest payload context that
			 * starts execution
			 */
			struct kkm_regs regs;
			struct kkm_sregs sregs;
			struct kkm_debug debug;
			struct kkm_fpu fpu;
			struct kkm_trap_info trap_info;

			/*
			 * current interrupt
			 */
			uint64_t intr_no;

			/*
			 * this padding makes sure
			 * the entry stack is aligned correctly
			 */
			uint8_t reserved[2328];

			/*
			 * copy buffer
			 * used for instruction decoding
			 */
			uint8_t instruction_decode[KKM_GUEST_COPY_BUFFER];

			/*
			 * first page of guest area(0xE00 - 0x1000)
			 * payload_entry_stack is used in kkm_guest_entry.S
			 */
			struct entry_stack
				payload_entry_stack; /* stack used for switching to payload */
		};
		uint8_t data[GUEST_PRIVATE_DATA_SIZE];
	};
	uint8_t redzone_top[GUEST_STACK_REDZONE_SIZE];
	uint8_t stack[GUEST_STACK_SIZE]; /* guest kernel stack */
	uint8_t redzone_bottom[GUEST_STACK_REDZONE_SIZE];
};
static_assert(sizeof(struct kkm_guest_area) == 8192, "Size is not correct");

int kkm_kontext_init(struct kkm_kontext *kkm_kontext);
void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext);
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext);
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga);
void kkm_switch_to_host_kernel(void);

void kkm_hw_debug_registers_save(uint64_t *registers);
void kkm_hw_debug_registers_restore(uint64_t *registers);

int kkm_process_intr(struct kkm_kontext *kkm_kontext);
void kkm_setup_hypercall(struct kkm_kontext *kkm_kontext,
			 struct kkm_guest_area *ga, struct kkm_run *kkm_run,
			 uint16_t port, uint32_t addr,
			 enum fault_reason reason);
int kkm_process_common_without_error(struct kkm_kontext *kkm_kontext,
				     struct kkm_guest_area *ga,
				     struct kkm_run *kkm_run);
int kkm_process_common_with_error(struct kkm_kontext *kkm_kontext,
				  struct kkm_guest_area *ga,
				  struct kkm_run *kkm_run);
int kkm_process_debug(struct kkm_kontext *kkm_kontext,
		      struct kkm_guest_area *ga, struct kkm_run *kkm_run);
int kkm_process_breakpoint(struct kkm_kontext *kkm_kontext,
			   struct kkm_guest_area *ga, struct kkm_run *kkm_run);
int kkm_process_general_protection(struct kkm_kontext *kkm_kontext,
				   struct kkm_guest_area *ga,
				   struct kkm_run *kkm_run);
int kkm_process_page_fault(struct kkm_kontext *kkm_kontext,
			   struct kkm_guest_area *ga, struct kkm_run *kkm_run);
int kkm_process_syscall(struct kkm_kontext *kkm_kontext,
			struct kkm_guest_area *ga, struct kkm_run *kkm_run);

bool kkm_guest_va_to_monitor_va(struct kkm_kontext *kkm_kontext,
				uint64_t guest_va, uint64_t *monitor_va,
				bool *priv_area);

#endif /* __KKM_KONTEXT_H__ */
