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

#include <linux/kernel.h>
#include <linux/atomic.h>

#include "kkm_trace.h"

static inline struct kkm_trace_entry *
kkm_trace_get_next_entry(struct kkm_trace *trace)
{
	int index = atomic64_inc_return(&trace->index) % KKM_TRACE_DEPTH;
	return &trace->entries[index];
}

void kkm_trace_init(struct kkm_trace *trace)
{
	int index = 0;

	atomic64_set(&trace->index, 0);
	for (index = 0; index < KKM_TRACE_DEPTH; index++) {
		trace->entries[index].type = KKM_TRACE_INVALID_ENTRY;
	}
}

void kkm_trace_show(struct kkm_trace *trace)
{
	int index = 0;
	s64 current_index = 0;
	struct kkm_trace_entry *entry;

	current_index = atomic64_read(&trace->index) + 1;
	printk(KERN_NOTICE "kkm_trace_show: trace index 0x%llx\n",
	       current_index);
	for (index = 0; index < KKM_TRACE_DEPTH; index++) {
		entry = &trace->entries[(current_index + index) %
					KKM_TRACE_DEPTH];
		if (entry->type == KKM_TRACE_INVALID_ENTRY) {
			continue;
		}
		switch (entry->type) {
		case KKM_TRACE_SET_REGS:
			printk(KERN_NOTICE
			       "kkm_trace_show: SET_REGS rip %llx rsp %llx\n",
			       entry->set_regs.rip, entry->set_regs.rsp);
			break;
		case KKM_TRACE_RUN:
			printk(KERN_NOTICE
			       "kkm_trace_show: RUN rip %llx rsp %llx\n",
			       entry->run.rip, entry->run.rsp);
			break;
		case KKM_TRACE_RUN_DONE:
			printk(KERN_NOTICE
			       "kkm_trace_show: RUN_DONE rip %llx rsp %llx\n",
			       entry->run.rip, entry->run.rsp);
			break;
		case KKM_TRACE_GUEST_EXIT:
			printk(KERN_NOTICE
			       "kkm_trace_show: GUEST_EXIT reason %x\n",
			       entry->guest_exit.reason);
			break;
		case KKM_TRACE_FORWARD:
			printk(KERN_NOTICE
			       "kkm_trace_show: FORWARD intr %x rip %llx rsp %llx\n",
			       entry->forward.intr, entry->forward.rip,
			       entry->forward.rsp);
			break;
		case KKM_TRACE_FORWARD_DONE:
			printk(KERN_NOTICE
			       "kkm_trace_show: FORWARD_DONE intr %x rip %llx rsp %llx\n",
			       entry->forward_done.intr,
			       entry->forward_done.rip,
			       entry->forward_done.rsp);
			break;
		case KKM_TRACE_PAGE_FAULT:
			printk(KERN_NOTICE
			       "kkm_trace_show: PAGE_FAULT cr2 %llx error %llx rip %llx rsp %llx\n",
			       entry->page_fault.cr2, entry->page_fault.error,
			       entry->page_fault.rip, entry->page_fault.rsp);
			break;
		case KKM_TRACE_PAGE_FAULT_DONE:
			printk(KERN_NOTICE
			       "kkm_trace_show: PAGE_FAULT_DONE cr2 %llx error %llx rip %llx rsp %llx\n",
			       entry->page_fault_done.cr2,
			       entry->page_fault_done.error,
			       entry->page_fault_done.rip,
			       entry->page_fault_done.rsp);
			break;
		default:
			printk(KERN_NOTICE "kkm_trace_show: UNKNOWN_ENTRY %x\n",
			       entry->type);
			break;
		}
	}
}

void kkm_trace_add_entry_set_regs(struct kkm_trace *trace, uint64_t rip,
				  uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_SET_REGS;
	entry->set_regs.rip = rip;
	entry->set_regs.rsp = rsp;
}

void kkm_trace_add_entry_run(struct kkm_trace *trace, uint64_t rip,
			     uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_RUN;
	entry->run.rip = rip;
	entry->run.rsp = rsp;
}

void kkm_trace_add_entry_run_done(struct kkm_trace *trace, uint64_t rip,
				  uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_RUN_DONE;
	entry->run.rip = rip;
	entry->run.rsp = rsp;
}

void kkm_trace_add_entry_guest_exit(struct kkm_trace *trace, uint32_t reason)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_GUEST_EXIT;
	entry->guest_exit.reason = reason;
}

void kkm_trace_add_entry_forward(struct kkm_trace *trace, uint32_t intr,
				 uint64_t rip, uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_FORWARD;
	entry->forward.intr = intr;
	entry->forward.rip = rip;
	entry->forward.rsp = rsp;
}

void kkm_trace_add_entry_forward_done(struct kkm_trace *trace, uint32_t intr,
				      uint64_t rip, uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_FORWARD_DONE;
	entry->forward_done.intr = intr;
	entry->forward_done.rip = rip;
	entry->forward_done.rsp = rsp;
}

void kkm_trace_add_entry_page_fault(struct kkm_trace *trace, uint64_t cr2,
				    uint64_t error, uint64_t rip, uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_PAGE_FAULT;
	entry->page_fault.cr2 = cr2;
	entry->page_fault.error = error;
	entry->page_fault.rip = rip;
	entry->page_fault.rsp = rsp;
}

void kkm_trace_add_entry_page_fault_done(struct kkm_trace *trace, uint64_t cr2,
					 uint64_t error, uint64_t rip,
					 uint64_t rsp)
{
	struct kkm_trace_entry *entry = kkm_trace_get_next_entry(trace);
	entry->type = KKM_TRACE_PAGE_FAULT_DONE;
	entry->page_fault_done.cr2 = cr2;
	entry->page_fault_done.error = error;
	entry->page_fault_done.rip = rip;
	entry->page_fault_done.rsp = rsp;
}
