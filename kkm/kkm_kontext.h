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

#define KKM_GUEST_AREA_PAGES	(2)
#define	KKM_GUEST_AREA_SIZE	(KKM_GUEST_AREA_PAGES * PAGE_SIZE)

#define GUEST_PRIVATE_DATA_SIZE (4096)
#define GUEST_STACK_REDZONE_SIZE (256)
#define GUEST_STACK_SIZE                                                       \
	(KKM_GUEST_AREA_SIZE - GUEST_PRIVATE_DATA_SIZE - GUEST_STACK_REDZONE_SIZE)

#define GUEST_STACK_START_ADDRESS(guest_area_start)                            \
	(guest_area_start + ((struct kkm_guest_area *)0)->redzone)

struct kkm_guest_area {
	// data store
	union {
		struct {
			struct kkm_kontext *kkm_kontext;
			uint64_t guest_area_beg; /* virtual address of this struct */
			uint64_t native_kernel_stack; /* %rsp before switching stacks */
			uint64_t guest_kernel_cr3;
			uint64_t guest_payload_cr3;
			uint64_t guest_stack_variable_address;
			uint64_t guest_payload_cs;
			uint64_t guest_payload_ss;

			struct kkm_regs regs;
			struct kkm_sregs sregs;
			struct kkm_debug debug;
			struct kkm_fpu fpu;

			uint8_t reserved[2472];

			// keep these two entries at the end
			// offset of payload_entry_stack is used
			// at offset(0x600-0x800) from begining of
			// this structure in kkm_entry.S
			struct entry_stack native_entry_stack;
			struct entry_stack payload_entry_stack;
		};
		char data[GUEST_PRIVATE_DATA_SIZE];
	};
	char stack[GUEST_STACK_SIZE];
	char redzone[GUEST_STACK_REDZONE_SIZE];
};
static_assert (sizeof(struct kkm_guest_area) == 8192, "Size is not correct");

int kkm_kontext_init(struct kkm_kontext *kkm_kontext);
void kkm_kontext_cleanup(struct kkm_kontext *kkm_kontext);
int kkm_kontext_switch_kernel(struct kkm_kontext *kkm_kontext);
void kkm_guest_kernel_start_payload(struct kkm_guest_area *ga);
void kkm_switch_to_host_kernel(void);

#endif /* KKM_KONTEXT_H__ */
