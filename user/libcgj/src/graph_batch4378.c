/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4378: period (ns) to frequency (Hz) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_period_ns_to_hz_u2(uint64_t period_ns);
 *     - Return floor(1e9 / period_ns) as frequency in hertz.
 *       period_ns == 0 → 0 (undefined reciprocal; defined sentinel).
 *   uint64_t __gj_period_ns_to_hz_u2  (alias)
 *   __libcgj_batch4378_marker = "libcgj-batch4378"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Inverse of
 * gj_hz_to_period_ns_u (batch4377). Note: symbol name also appears in
 * batch3141 with identical semantics — coordinate link set to avoid
 * multi-def if both TUs are archived together. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4378_marker[] = "libcgj-batch4378";

/* Nanoseconds in one second (Hz scale for period→frequency). */
#define B4378_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(1e9 / period_ns); 0 when period_ns is 0. */
static uint64_t
b4378_period_to_hz(uint64_t u64PeriodNs)
{
	if (u64PeriodNs == 0ull) {
		return 0ull;
	}
	return B4378_NS_PER_S / u64PeriodNs;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_period_ns_to_hz_u2 - convert a nanosecond period to frequency in Hz.
 *
 * period_ns: positive period in nanoseconds (0 → 0 sentinel).
 *
 * Returns floor(1000000000 / period_ns). Integer-only; no floating point.
 * Self-contained; no parent wires.
 */
uint64_t
gj_period_ns_to_hz_u2(uint64_t u64PeriodNs)
{
	(void)NULL;
	return b4378_period_to_hz(u64PeriodNs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_period_ns_to_hz_u2(uint64_t u64PeriodNs)
    __attribute__((alias("gj_period_ns_to_hz_u2")));
