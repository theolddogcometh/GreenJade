/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4376: microseconds to nanoseconds saturating (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_us_to_ns_sat_u(uint64_t us);
 *     - Return us * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_us_to_ns_sat_u  (alias)
 *   __libcgj_batch4376_marker = "libcgj-batch4376"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Inverse of
 * gj_ns_to_us_u (batch4375). Distinct from gj_us_to_ns (batch1587) —
 * unique gj_us_to_ns_sat_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4376_marker[] = "libcgj-batch4376";

/* Nanoseconds per microsecond (SI: 1 us = 1000 ns). */
#define B4376_NS_PER_US  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B4376_NS_PER_US. */
static uint64_t
b4376_mul_ns(uint64_t u64Us)
{
	if (u64Us > (UINT64_MAX / B4376_NS_PER_US)) {
		return UINT64_MAX;
	}
	return u64Us * B4376_NS_PER_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_us_to_ns_sat_u - convert microseconds to nanoseconds (saturating).
 *
 * us: microseconds
 *
 * Returns us * 1000, or UINT64_MAX if the product would overflow.
 * Self-contained; no parent wires.
 */
uint64_t
gj_us_to_ns_sat_u(uint64_t u64Us)
{
	(void)NULL;
	return b4376_mul_ns(u64Us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_us_to_ns_sat_u(uint64_t u64Us)
    __attribute__((alias("gj_us_to_ns_sat_u")));
