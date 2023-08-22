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

#ifndef __KKM_STATISTICS_H__
#define __KKM_STATISTICS_H__

struct kkm_statistics {
	atomic64_t kontainer_count;
	atomic64_t kontext_count;
	atomic64_t intr_count;
	atomic64_t forwarded_intr_count;
	atomic64_t forwarded_intr_time_ns;
	atomic64_t read_page_fault_count;
	atomic64_t write_page_fault_count;
	atomic64_t failed_page_fault_count;
	atomic64_t page_fault_time_ns;
	atomic64_t system_call_count;
	atomic64_t payload_entry_count;
	atomic64_t payload_time_ns;
	atomic64_t payload_gk_time_ns;
	atomic64_t debug_count;
	atomic64_t debug_save_time_ns;
	atomic64_t debug_restore_time_ns;
};

extern struct kkm_statistics kkm_stat;

static inline void kkm_statistics_init(void)
{
	atomic64_set(&kkm_stat.kontainer_count, 0);
	atomic64_set(&kkm_stat.kontext_count, 0);
	atomic64_set(&kkm_stat.intr_count, 0);
	atomic64_set(&kkm_stat.forwarded_intr_count, 0);
	atomic64_set(&kkm_stat.forwarded_intr_time_ns, 0);
	atomic64_set(&kkm_stat.read_page_fault_count, 0);
	atomic64_set(&kkm_stat.write_page_fault_count, 0);
	atomic64_set(&kkm_stat.failed_page_fault_count, 0);
	atomic64_set(&kkm_stat.page_fault_time_ns, 0);
	atomic64_set(&kkm_stat.system_call_count, 0);
	atomic64_set(&kkm_stat.payload_entry_count, 0);
	atomic64_set(&kkm_stat.payload_time_ns, 0);
	atomic64_set(&kkm_stat.payload_gk_time_ns, 0);
	atomic64_set(&kkm_stat.debug_count, 0);
	atomic64_set(&kkm_stat.debug_save_time_ns, 0);
	atomic64_set(&kkm_stat.debug_restore_time_ns, 0);
}

static inline int kkm_statistics_show(char *s)
{
	return sprintf(s,
		       "kontainers\t: %lld\n"
		       "kontexts\t: %lld\n"
		       "interrupts\t: %lld\n"
		       "forwarded intr\t: %lld\n"
		       "forwarded intr time ns\t: %lld\n"
		       "read page faults\t: %lld\n"
		       "write page faults\t: %lld\n"
		       "failed page faults\t: %lld\n"
		       "page fault time ns\t: %lld\n"
		       "system calls\t: %lld\n"
		       "payload entry count\t: %lld\n"
		       "payload time ns\t: %lld\n"
		       "payload gk time ns\t: %lld\n"
		       "debug count\t: %lld\n"
		       "debug save time ns\t: %lld\n"
		       "debug restore time ns\t: %lld\n",
		       atomic64_read(&kkm_stat.kontainer_count),
		       atomic64_read(&kkm_stat.kontext_count),
		       atomic64_read(&kkm_stat.intr_count),
		       atomic64_read(&kkm_stat.forwarded_intr_count),
		       atomic64_read(&kkm_stat.forwarded_intr_time_ns),
		       atomic64_read(&kkm_stat.read_page_fault_count),
		       atomic64_read(&kkm_stat.write_page_fault_count),
		       atomic64_read(&kkm_stat.failed_page_fault_count),
		       atomic64_read(&kkm_stat.page_fault_time_ns),
		       atomic64_read(&kkm_stat.system_call_count),
		       atomic64_read(&kkm_stat.payload_entry_count),
		       atomic64_read(&kkm_stat.payload_time_ns),
		       atomic64_read(&kkm_stat.payload_gk_time_ns),
		       atomic64_read(&kkm_stat.debug_count),
		       atomic64_read(&kkm_stat.debug_save_time_ns),
		       atomic64_read(&kkm_stat.debug_restore_time_ns));
}

static inline void kkm_statistics_kontainer_count_inc(void)
{
	atomic64_inc(&kkm_stat.kontainer_count);
}

static inline void kkm_statistics_kontext_count_inc(void)
{
	atomic64_inc(&kkm_stat.kontext_count);
}

static inline void kkm_statistics_intr_count_inc(void)
{
	atomic64_inc(&kkm_stat.intr_count);
}

static inline void kkm_statistics_forwarded_intr_count_inc(void)
{
	atomic64_inc(&kkm_stat.forwarded_intr_count);
}

static inline void kkm_statistics_forwarded_intr_time_ns(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.forwarded_intr_time_ns);
}

static inline void kkm_statistics_read_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.read_page_fault_count);
}

static inline void kkm_statistics_write_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.write_page_fault_count);
}

static inline void kkm_statistics_failed_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.failed_page_fault_count);
}

static inline void kkm_statistics_page_fault_time_ns_add(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.page_fault_time_ns);
}

static inline void kkm_statistics_system_call_count_inc(void)
{
	atomic64_inc(&kkm_stat.system_call_count);
}

static inline void kkm_statistics_payload_entry_count_inc(void)
{
	atomic64_inc(&kkm_stat.payload_entry_count);
}

static inline void kkm_statistics_payload_time_ns(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.payload_time_ns);
}

static inline void kkm_statistics_payload_gk_time_ns(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.payload_gk_time_ns);
}

static inline void kkm_statistics_debug_count_inc(void)
{
	atomic64_inc(&kkm_stat.debug_count);
}

static inline void kkm_statistics_debug_save_time_ns(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.debug_save_time_ns);
}

static inline void kkm_statistics_debug_restore_time_ns(uint64_t ns)
{
	atomic64_add(ns, &kkm_stat.debug_restore_time_ns);
}

#endif /* __KKM_STATISTICS_H__ */
