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
	atomic64_t page_fault_count;
	atomic64_t failed_page_fault_count;
	atomic64_t page_fault_time_ns;
	atomic64_t system_call_count;
	atomic64_t pf_hash_distribution[KKM_PF_HASH_BUCKETS];
};

extern struct kkm_statistics kkm_stat;

static inline void kkm_statistics_init(void)
{
	int i = 0;
	atomic64_set(&kkm_stat.kontainer_count, 0);
	atomic64_set(&kkm_stat.kontext_count, 0);
	atomic64_set(&kkm_stat.intr_count, 0);
	atomic64_set(&kkm_stat.forwarded_intr_count, 0);
	atomic64_set(&kkm_stat.forwarded_intr_time_ns, 0);
	atomic64_set(&kkm_stat.page_fault_count, 0);
	atomic64_set(&kkm_stat.failed_page_fault_count, 0);
	atomic64_set(&kkm_stat.page_fault_time_ns, 0);
	atomic64_set(&kkm_stat.system_call_count, 0);
	for (i = 0; i < KKM_PF_HASH_BUCKETS; i++) {
		atomic64_set(&kkm_stat.pf_hash_distribution[i], 0);
	}
}

static inline int kkm_statistics_show(char *s)
{
	int i = 0;
	int count = sprintf(s,
			    "kontainers\t: %lld\n"
			    "kontexts\t: %lld\n"
			    "interrupts\t: %lld\n"
			    "forwarded intr\t: %lld\n"
			    "forwarded intr time ns\t: %lld\n"
			    "page faults\t: %lld\n"
			    "failed page faults\t: %lld\n"
			    "page fault time ns\t: %lld\n"
			    "system calls\t: %lld\n",
			    atomic64_read(&kkm_stat.kontainer_count),
			    atomic64_read(&kkm_stat.kontext_count),
			    atomic64_read(&kkm_stat.intr_count),
			    atomic64_read(&kkm_stat.forwarded_intr_count),
			    atomic64_read(&kkm_stat.forwarded_intr_time_ns),
			    atomic64_read(&kkm_stat.page_fault_count),
			    atomic64_read(&kkm_stat.failed_page_fault_count),
			    atomic64_read(&kkm_stat.page_fault_time_ns),
			    atomic64_read(&kkm_stat.system_call_count));
	count += sprintf(&s[count], "pf hash dist\t:");
	for (i = 0; i < KKM_PF_HASH_BUCKETS; i++) {
		count += sprintf(
			&s[count], " %lld",
			atomic64_read(&kkm_stat.pf_hash_distribution[i]));
	}
	count += sprintf(&s[count], "\n");

	return count;
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

static inline void kkm_statistics_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.page_fault_count);
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

static inline void kkm_statistics_pf_hash_distribution_inc(int i)
{
	atomic64_inc(&kkm_stat.pf_hash_distribution[i]);
}

#endif /* __KKM_STATISTICS_H__ */
