/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1719: midpoint of a half-open uint64 interval.
 *
 * Surface (unique symbols):
 *   uint64_t gj_interval_mid_u64(uint64_t lo, uint64_t hi);
 *     — Half-open interval [lo, hi). Returns lo + floor((hi - lo) / 2)
 *       when lo < hi (floor midpoint toward lo). Empty (lo >= hi)
 *       returns lo. Overflow-safe for full-span ranges.
 *   uint64_t __gj_interval_mid_u64  (alias)
 *   __libcgj_batch1719_marker = "libcgj-batch1719"
 *
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1719_marker[] = "libcgj-batch1719";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor midpoint without (lo+hi)/2 overflow. */
static uint64_t
b1719_mid(uint64_t u64Lo, uint64_t u64Hi)
{
	return u64Lo + ((u64Hi - u64Lo) / 2ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_interval_mid_u64 — floor midpoint of half-open [lo, hi).
 *
 * Empty intervals return lo. Non-empty return a value in [lo, hi).
 */
uint64_t
gj_interval_mid_u64(uint64_t u64Lo, uint64_t u64Hi)
{
	if (u64Lo >= u64Hi) {
		return u64Lo;
	}
	return b1719_mid(u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_interval_mid_u64(uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_interval_mid_u64")));
