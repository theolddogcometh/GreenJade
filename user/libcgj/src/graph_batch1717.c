/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1717: clamp a uint64 value into a half-open
 * interval's closed span.
 *
 * Surface (unique symbols):
 *   uint64_t gj_interval_clamp_u64(uint64_t x, uint64_t lo, uint64_t hi);
 *     — Half-open interval [lo, hi). Returns x clamped into the closed
 *       span of representable members [lo, hi-1] when lo < hi.
 *       Empty interval (lo >= hi) returns lo. No overflow on hi==0 empty.
 *   uint64_t __gj_interval_clamp_u64  (alias)
 *   __libcgj_batch1717_marker = "libcgj-batch1717"
 *
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1717_marker[] = "libcgj-batch1717";

/* ---- freestanding helpers ---------------------------------------------- */

/* Last valid inclusive member of half-open [lo, hi): hi - 1 (hi > lo). */
static uint64_t
b1717_last(uint64_t u64Hi)
{
	return u64Hi - 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_interval_clamp_u64 — clamp x into half-open [lo, hi) as closed [lo, hi-1].
 *
 * lo: inclusive lower bound of the interval
 * hi: exclusive upper bound of the interval
 * x:  value to clamp
 *
 * Returns lo if empty; otherwise min(max(x, lo), hi-1).
 */
uint64_t
gj_interval_clamp_u64(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Last;

	if (u64Lo >= u64Hi) {
		return u64Lo;
	}

	u64Last = b1717_last(u64Hi);
	if (u64X < u64Lo) {
		return u64Lo;
	}
	if (u64X > u64Last) {
		return u64Last;
	}
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_interval_clamp_u64(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_interval_clamp_u64")));
