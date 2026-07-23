/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1748: CFS quota period validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_quota_period_ok(uint64_t period_us);
 *     — Return 1 if period_us is in the classic CFS bandwidth period
 *       window [1000, 1000000] microseconds (1 ms .. 1 s), else 0.
 *   int __gj_quota_period_ok  (alias)
 *   __libcgj_batch1748_marker = "libcgj-batch1748"
 *
 * Pure data — not a cpu.cfs_period_us writer. Does NOT redefine prior
 * batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1748_marker[] = "libcgj-batch1748";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1748_ok(uint64_t u64PeriodUs)
{
	if (u64PeriodUs < 1000ull) {
		return 0;
	}
	if (u64PeriodUs > 1000000ull) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_quota_period_ok — 1 if period_us is a valid CFS bandwidth period.
 *
 * Screens parsed cpu.cfs_period_us / cpu.max period fields before
 * quota math. Pure predicate; no sched_entity, no cgroupfs.
 */
int
gj_quota_period_ok(uint64_t u64PeriodUs)
{
	return b1748_ok(u64PeriodUs);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_quota_period_ok(uint64_t u64PeriodUs)
    __attribute__((alias("gj_quota_period_ok")));
