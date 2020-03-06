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

struct kkm_trap_info {
	uint64_t ss;
	uint64_t rsp;
	uint64_t rflags;
	uint64_t cs;
	uint64_t rip;
	uint64_t error;
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
			uint64_t guest_stack_variable_address; /* debug variable */

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

			uint8_t reserved1[16];

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

			uint64_t kkm_intr_no;

			uint8_t reserved[2352];

			/* keep these two entries at the end
			 * first page of guest area(0xE00 - 0x1000)
			 * payload_entry_stack is used in kkm_entry.S
			 */
			struct entry_stack
				native_entry_stack; /* native kernel iretq stack save area */
			struct entry_stack
				payload_entry_stack; /* stack used for switching to payload */
		};
		char data[GUEST_PRIVATE_DATA_SIZE];
	};
	char redzone_top[GUEST_STACK_REDZONE_SIZE];
	char stack[GUEST_STACK_SIZE]; /* guest kernel stack */
	char redzone_bottom[GUEST_STACK_REDZONE_SIZE];
};
static_assert(sizeof(struct kkm_guest_area) == 8192, "Size is not correct");

int kkm_kontext_init(struct kkm_kontext *kkm_kontext);
void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext);
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext);
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga);
void kkm_switch_to_host_kernel(void);

#endif /* KKM_KONTEXT_H__ */
