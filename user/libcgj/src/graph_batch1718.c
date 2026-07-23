/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1718: half-open uint64 interval width.
 *
 * Surface (unique symbols):
 *   uint64_t gj_interval_width_u64(uint64_t lo, uint64_t hi);
 *     — Half-open interval [lo, hi). Returns hi - lo when lo < hi,
 *       else 0 (empty). Difference is exact in uint64_t when lo < hi.
 *   uint64_t __gj_interval_width_u64  (alias)
 *   __libcgj_batch1718_marker = "libcgj-batch1718"
 *
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1718_marker[] = "libcgj-batch1718";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1718_nonempty(uint64_t u64Lo, uint64_t u64Hi)
{
	return (u64Lo < u64Hi) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_interval_width_u64 — measure of half-open [lo, hi).
 *
 * Empty when lo >= hi → 0. Otherwise hi - lo (number of integers in range).
 */
uint64_t
gj_interval_width_u64(uint64_t u64Lo, uint64_t u64Hi)
{
	if (b1718_nonempty(u64Lo, u64Hi) == 0) {
		return 0ull;
	}
	return u64Hi - u64Lo;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_interval_width_u64(uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_interval_width_u64")));
