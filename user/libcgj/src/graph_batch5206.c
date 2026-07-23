/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5206: timer period nanoseconds to Hz (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timer_period_ns_to_hz_u(uint64_t period);
 *     - Return floor(1000000000 / period) as frequency in hertz.
 *       period == 0 → 0 (undefined reciprocal; defined sentinel).
 *   uint64_t __gj_timer_period_ns_to_hz_u  (alias)
 *   __libcgj_batch5206_marker = "libcgj-batch5206"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_timer_period_ns_to_hz_u surface only; no multi-def.
 * Inverse of gj_timer_hz_to_period_ns_u (batch5205). Distinct from
 * gj_period_ns_to_hz_u (batch3141) and gj_period_ns_to_hz_u2
 * (batch4378). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5206_marker[] = "libcgj-batch5206";

/* Nanoseconds in one second (Hz scale for period→frequency). */
#define B5206_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(1e9 / period); 0 when period is 0. */
static uint64_t
b5206_period_to_hz(uint64_t u64Period)
{
	if (u64Period == 0ull) {
		return 0ull;
	}
	return B5206_NS_PER_S / u64Period;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timer_period_ns_to_hz_u - convert a nanosecond period to frequency.
 *
 * period: positive period in nanoseconds (0 → 0 sentinel)
 *
 * Returns floor(1000000000 / period). Integer-only; no floating point.
 * No parent wires.
 */
uint64_t
gj_timer_period_ns_to_hz_u(uint64_t u64Period)
{
	(void)NULL;
	return b5206_period_to_hz(u64Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timer_period_ns_to_hz_u(uint64_t u64Period)
    __attribute__((alias("gj_timer_period_ns_to_hz_u")));
