/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1587: microseconds to nanoseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_us_to_ns(uint64_t us);
 *     — Return us * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_us_to_ns  (alias)
 *   __libcgj_batch1587_marker = "libcgj-batch1587"
 *
 * Distinct from gj_time_us_to_ms (batch1034) and gj_ms_to_ns
 * (batch1586) — explicit us→ns surface only. No multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1587_marker[] = "libcgj-batch1587";

/* Nanoseconds per microsecond (SI: 1 us = 1e3 ns). */
#define B1587_NS_PER_US  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1587_NS_PER_US. */
static uint64_t
b1587_mul_us(uint64_t u64Us)
{
	if (u64Us > (UINT64_MAX / B1587_NS_PER_US)) {
		return UINT64_MAX;
	}
	return u64Us * B1587_NS_PER_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_us_to_ns — convert microseconds to nanoseconds (saturating).
 *
 * u64Us: microseconds
 * Returns u64Us * 1000, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_us_to_ns(uint64_t u64Us)
{
	return b1587_mul_us(u64Us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_us_to_ns(uint64_t u64Us)
    __attribute__((alias("gj_us_to_ns")));
