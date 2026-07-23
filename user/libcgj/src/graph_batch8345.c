/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8345: microseconds to nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_us_to_ns_u_8345(uint64_t us);
 *     - Return us * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_us_to_ns_u_8345  (alias)
 *   __libcgj_batch8345_marker = "libcgj-batch8345"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Distinct from gj_us_to_ns (batch1587) and gj_us_to_ns_sat_u
 * (batch4376) — unique gj_us_to_ns_u_8345 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8345_marker[] = "libcgj-batch8345";

/* Nanoseconds per microsecond (SI: 1 us = 1000 ns). */
#define B8345_NS_PER_US  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b8345_us_to_ns(uint64_t u64Us)
{
	if (u64Us > (UINT64_MAX / B8345_NS_PER_US)) {
		return UINT64_MAX;
	}
	return u64Us * B8345_NS_PER_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_us_to_ns_u_8345 - convert microseconds to nanoseconds (saturating).
 *
 * us: microseconds
 *
 * Returns us * 1000, or UINT64_MAX if the product would overflow.
 * Soft pure-integer unit stub; no parent wires.
 */
uint64_t
gj_us_to_ns_u_8345(uint64_t us)
{
	(void)NULL;
	return b8345_us_to_ns(us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_us_to_ns_u_8345(uint64_t us)
    __attribute__((alias("gj_us_to_ns_u_8345")));
