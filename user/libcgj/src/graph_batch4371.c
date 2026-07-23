/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4371: milliseconds to microseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_us_u(uint64_t ms);
 *     - Return ms * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_ms_to_us_u  (alias)
 *   __libcgj_batch4371_marker = "libcgj-batch4371"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380: ms_to_us_u, us_to_ms_u,
 * sec_to_ms_u, ms_to_sec_u, ns_to_us_u, us_to_ns_sat_u,
 * hz_to_period_ns_u, period_ns_to_hz_u, ticks_to_ns_u, batch_id_4380).
 * Distinct from gj_ms_to_us (batch1971) and gj_time_ms_to_us
 * (batch1033) — unique gj_ms_to_us_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4371_marker[] = "libcgj-batch4371";

/* Microseconds per millisecond (SI: 1 ms = 1000 us). */
#define B4371_US_PER_MS  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4371_US_PER_MS. */
static uint64_t
b4371_mul_us(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / B4371_US_PER_MS)) {
		return UINT64_MAX;
	}
	return u64Ms * B4371_US_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_us_u - convert milliseconds to microseconds (saturating).
 *
 * ms: milliseconds
 *
 * Returns ms * 1000, or UINT64_MAX if the product would overflow.
 * Self-contained; no parent wires.
 */
uint64_t
gj_ms_to_us_u(uint64_t u64Ms)
{
	(void)NULL;
	return b4371_mul_us(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_us_u(uint64_t u64Ms)
    __attribute__((alias("gj_ms_to_us_u")));
