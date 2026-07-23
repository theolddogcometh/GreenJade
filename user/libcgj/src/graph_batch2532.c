/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2532: minutes to seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_min_to_sec(uint64_t min);
 *     — Return min * 60, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_min_to_sec  (alias)
 *   __libcgj_batch2532_marker = "libcgj-batch2532"
 *
 * Time exclusive pure-data wave (2531–2540). Inverse of
 * gj_sec_to_min_floor (batch2531). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2532_marker[] = "libcgj-batch2532";

/* Seconds per minute (SI: 1 min = 60 s). */
#define B2532_SEC_PER_MIN  60ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B2532_SEC_PER_MIN. */
static uint64_t
b2532_mul_sec(uint64_t u64Min)
{
	if (u64Min > (UINT64_MAX / B2532_SEC_PER_MIN)) {
		return UINT64_MAX;
	}
	return u64Min * B2532_SEC_PER_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_min_to_sec — convert whole minutes to seconds (saturating).
 *
 * min: minutes
 * Returns min * 60, or UINT64_MAX if the product would overflow.
 * Examples: 0 → 0, 1 → 60, 2 → 120.
 */
uint64_t
gj_min_to_sec(uint64_t u64Min)
{
	(void)NULL;
	return b2532_mul_sec(u64Min);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_min_to_sec(uint64_t u64Min)
    __attribute__((alias("gj_min_to_sec")));
