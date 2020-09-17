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

#ifndef __KKM_STATISTICS_H__
#define __KKM_STATISTICS_H__

struct kkm_statistics {
	atomic64_t kontainer_count;
	atomic64_t kontext_count;
	atomic64_t intr_count;
	atomic64_t forwarded_intr_count;
	atomic64_t page_fault_count;
	atomic64_t failed_page_fault_count;
	atomic64_t system_call_count;
};

extern struct kkm_statistics kkm_stat;

static inline void kkm_statistics_init(void)
{
	atomic64_set(&kkm_stat.kontainer_count, 0);
	atomic64_set(&kkm_stat.kontext_count, 0);
	atomic64_set(&kkm_stat.intr_count, 0);
	atomic64_set(&kkm_stat.forwarded_intr_count, 0);
	atomic64_set(&kkm_stat.page_fault_count, 0);
	atomic64_set(&kkm_stat.failed_page_fault_count, 0);
	atomic64_set(&kkm_stat.system_call_count, 0);
}

static inline int kkm_statistics_show(char *s)
{
	return sprintf(s,
		       "kontainers\t:%lld\nkontexts\t:%lld\ninterrupts\t:%lld\n"
		       "forwarded intr\t:%lld\npage faults\t:%lld\n"
		       "failed page faults\t:%lld\nsystem calls\t:%lld\n",
		       atomic64_read(&kkm_stat.kontainer_count),
		       atomic64_read(&kkm_stat.kontext_count),
		       atomic64_read(&kkm_stat.intr_count),
		       atomic64_read(&kkm_stat.forwarded_intr_count),
		       atomic64_read(&kkm_stat.page_fault_count),
		       atomic64_read(&kkm_stat.failed_page_fault_count),
		       atomic64_read(&kkm_stat.system_call_count));
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

static inline void kkm_statistics_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.page_fault_count);
}

static inline void kkm_statistics_failed_page_fault_count_inc(void)
{
	atomic64_inc(&kkm_stat.failed_page_fault_count);
}

static inline void kkm_statistics_system_call_count_inc(void)
{
	atomic64_inc(&kkm_stat.system_call_count);
}

#endif /* __KKM_STATISTICS_H__ */
