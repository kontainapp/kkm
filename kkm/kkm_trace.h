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

#ifndef __KKM_TRACE_H__
#define __KKM_TRACE_H__

enum kkm_trace_type {
	KKM_TRACE_SET_REGS = 1,
	KKM_TRACE_RUN,
	KKM_TRACE_RUN_DONE,
	KKM_TRACE_GUEST_EXIT,
	KKM_TRACE_FORWARD,
	KKM_TRACE_FORWARD_DONE,
	KKM_TRACE_PAGE_FAULT,
	KKM_TRACE_PAGE_FAULT_DONE,
	KKM_TRACE_INVALID_ENTRY = -1
};

struct kkm_trace_entry {
	enum kkm_trace_type type;
	union {
		struct {
			uint64_t rip;
			uint64_t rsp;
		} set_regs;
		struct {
			uint64_t rip;
			uint64_t rsp;
		} run;
		struct {
			uint64_t rip;
			uint64_t rsp;
		} run_done;
		struct {
			uint32_t reason;
		} guest_exit;
		struct {
			uint32_t intr;
			uint64_t rip;
			uint64_t rsp;
		} forward;
		struct {
			uint32_t intr;
			uint64_t rip;
			uint64_t rsp;
		} forward_done;
		struct {
			uint64_t cr2;
			uint64_t error;
			uint64_t rip;
			uint64_t rsp;
		} page_fault;
		struct {
			uint64_t cr2;
			uint64_t error;
			uint64_t rip;
			uint64_t rsp;
		} page_fault_done;
	};
};

#define KKM_TRACE_DEPTH (0x100)

struct kkm_trace {
	atomic64_t index;
	struct kkm_trace_entry entries[KKM_TRACE_DEPTH];
};

void kkm_trace_init(struct kkm_trace *trace);
void kkm_trace_show(struct kkm_trace *trace);
void kkm_trace_add_entry_set_regs(struct kkm_trace *trace, uint64_t rip,
				  uint64_t rsp);
void kkm_trace_add_entry_run(struct kkm_trace *trace, uint64_t rip,
			     uint64_t rsp);
void kkm_trace_add_entry_run_done(struct kkm_trace *trace, uint64_t rip,
				  uint64_t rsp);
void kkm_trace_add_entry_guest_exit(struct kkm_trace *trace, uint32_t reason);
void kkm_trace_add_entry_forward(struct kkm_trace *trace, uint32_t intr,
				 uint64_t rip, uint64_t rsp);
void kkm_trace_add_entry_forward_done(struct kkm_trace *trace, uint32_t intr,
				      uint64_t rip, uint64_t rsp);
void kkm_trace_add_entry_page_fault(struct kkm_trace *trace, uint64_t cr2,
				    uint64_t error, uint64_t rip, uint64_t rsp);
void kkm_trace_add_entry_page_fault_done(struct kkm_trace *trace, uint64_t cr2,
					 uint64_t error, uint64_t rip,
					 uint64_t rsp);

#endif /* __KKM_TRACE_H__ */
