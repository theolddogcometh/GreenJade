/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1586: milliseconds to nanoseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_ns(uint64_t ms);
 *     — Return ms * 1000000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_ms_to_ns  (alias)
 *   __libcgj_batch1586_marker = "libcgj-batch1586"
 *
 * Distinct from gj_time_ms_to_us (batch1033) and gj_secs_to_ms
 * (batch484) — explicit ms→ns surface only. No multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1586_marker[] = "libcgj-batch1586";

/* Nanoseconds per millisecond (SI: 1 ms = 1e6 ns). */
#define B1586_NS_PER_MS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1586_NS_PER_MS. */
static uint64_t
b1586_mul_ms(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / B1586_NS_PER_MS)) {
		return UINT64_MAX;
	}
	return u64Ms * B1586_NS_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_ns — convert milliseconds to nanoseconds (saturating).
 *
 * u64Ms: milliseconds
 * Returns u64Ms * 1000000, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_ms_to_ns(uint64_t u64Ms)
{
	return b1586_mul_ms(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_ns(uint64_t u64Ms)
    __attribute__((alias("gj_ms_to_ns")));
