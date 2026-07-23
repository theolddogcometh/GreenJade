/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3141: period (ns) to frequency (Hz).
 *
 * Surface (unique symbols):
 *   uint64_t gj_period_ns_to_hz_u(uint64_t period_ns);
 *     - Return floor(1e9 / period_ns) as frequency in hertz.
 *       period_ns == 0 → 0 (undefined reciprocal; defined sentinel).
 *   uint64_t __gj_period_ns_to_hz_u  (alias)
 *   __libcgj_batch3141_marker = "libcgj-batch3141"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Unique
 * gj_period_ns_to_hz_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3141_marker[] = "libcgj-batch3141";

/* Nanoseconds in one second (Hz scale for period→frequency). */
#define B3141_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(1e9 / period_ns); 0 when period_ns is 0. */
static uint64_t
b3141_period_to_hz(uint64_t u64PeriodNs)
{
	if (u64PeriodNs == 0ull) {
		return 0ull;
	}
	return B3141_NS_PER_S / u64PeriodNs;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_period_ns_to_hz_u - convert a nanosecond period to frequency in Hz.
 *
 * period_ns: positive period in nanoseconds (0 → 0 sentinel).
 *
 * Returns floor(1000000000 / period_ns). Integer-only; no floating point.
 * No parent wires.
 */
uint64_t
gj_period_ns_to_hz_u(uint64_t u64PeriodNs)
{
	(void)NULL;
	return b3141_period_to_hz(u64PeriodNs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_period_ns_to_hz_u(uint64_t u64PeriodNs)
    __attribute__((alias("gj_period_ns_to_hz_u")));
