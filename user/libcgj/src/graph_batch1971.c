/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1971: milliseconds to microseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_us(uint64_t ms);
 *     — Return ms * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_ms_to_us  (alias)
 *   __libcgj_batch1971_marker = "libcgj-batch1971"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Distinct from
 * gj_time_ms_to_us (batch1033) — unique name. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1971_marker[] = "libcgj-batch1971";

/* Microseconds per millisecond (SI: 1 ms = 1000 us). */
#define B1971_US_PER_MS  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B1971_US_PER_MS. */
static uint64_t
b1971_mul_us(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / B1971_US_PER_MS)) {
		return UINT64_MAX;
	}
	return u64Ms * B1971_US_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_us — convert milliseconds to microseconds (saturating).
 *
 * ms: milliseconds
 * Returns ms * 1000, or UINT64_MAX if the product would overflow.
 */
uint64_t
gj_ms_to_us(uint64_t u64Ms)
{
	(void)NULL;
	return b1971_mul_us(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_us(uint64_t u64Ms)
    __attribute__((alias("gj_ms_to_us")));
