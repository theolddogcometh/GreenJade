/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1588: seconds to nanoseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_s_to_ns(uint64_t s);
 *     — Return s * 1000000000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_s_to_ns  (alias)
 *   __libcgj_batch1588_marker = "libcgj-batch1588"
 *
 * Distinct from gj_time_sec_to_ns (batch1031) — short-name SI surface.
 * Does NOT redefine prior time converters — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1588_marker[] = "libcgj-batch1588";

/* Nanoseconds per second (SI: 1 s = 1e9 ns). */
#define B1588_NS_PER_S  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1588_NS_PER_S. */
static uint64_t
b1588_mul_s(uint64_t u64S)
{
	if (u64S > (UINT64_MAX / B1588_NS_PER_S)) {
		return UINT64_MAX;
	}
	return u64S * B1588_NS_PER_S;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_s_to_ns — convert whole seconds to nanoseconds (saturating).
 *
 * u64S: seconds
 * Returns u64S * 1000000000, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_s_to_ns(uint64_t u64S)
{
	return b1588_mul_s(u64S);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_s_to_ns(uint64_t u64S)
    __attribute__((alias("gj_s_to_ns")));
