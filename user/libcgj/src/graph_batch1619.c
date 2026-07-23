/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1619: CPU percent from busy/total ticks.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpu_pct_from_ticks(uint64_t busy, uint64_t total);
 *     — busy * 100 / total, clamped to 100. total == 0 → 0. If busy
 *       exceeds total the result is 100 (over-accounted samples).
 *   uint32_t __gj_cpu_pct_from_ticks  (alias)
 *   __libcgj_batch1619_marker = "libcgj-batch1619"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1619_marker[] = "libcgj-batch1619";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_pct_from_ticks — integer CPU utilization percentage 0..100.
 *
 * Pure accounting helper for scheduler / job telemetry. Tick units are
 * opaque (jiffies, cycles, ns deltas) as long as both share a scale.
 */
uint32_t
gj_cpu_pct_from_ticks(uint64_t u64Busy, uint64_t u64Total)
{
	uint64_t u64Pct;

	if (u64Total == 0ull) {
		return 0u;
	}
	if (u64Busy >= u64Total) {
		return 100u;
	}

	/* busy * 100 fits when busy < 2^64/100; else divide-first path. */
	if (u64Busy > UINT64_MAX / 100ull) {
		u64Pct = (u64Busy / u64Total) * 100ull +
		    ((u64Busy % u64Total) * 100ull) / u64Total;
	} else {
		u64Pct = (u64Busy * 100ull) / u64Total;
	}
	if (u64Pct > 100ull) {
		return 100u;
	}
	return (uint32_t)u64Pct;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpu_pct_from_ticks(uint64_t u64Busy, uint64_t u64Total)
    __attribute__((alias("gj_cpu_pct_from_ticks")));
