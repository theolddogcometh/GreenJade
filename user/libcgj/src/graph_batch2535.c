/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2535: days to seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_day_to_sec(uint64_t day);
 *     — Return day * 86400, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_day_to_sec  (alias)
 *   __libcgj_batch2535_marker = "libcgj-batch2535"
 *
 * Time exclusive pure-data wave (2531–2540). Leap seconds ignored
 * (every day has 86400 SI seconds). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2535_marker[] = "libcgj-batch2535";

/* Seconds per day (SI: 1 d = 86400 s; no leap seconds). */
#define B2535_SEC_PER_DAY  86400ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B2535_SEC_PER_DAY. */
static uint64_t
b2535_mul_sec(uint64_t u64Day)
{
	if (u64Day > (UINT64_MAX / B2535_SEC_PER_DAY)) {
		return UINT64_MAX;
	}
	return u64Day * B2535_SEC_PER_DAY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_day_to_sec — convert whole days to seconds (saturating).
 *
 * day: days
 * Returns day * 86400, or UINT64_MAX if the product would overflow.
 * Examples: 0 → 0, 1 → 86400, 2 → 172800.
 */
uint64_t
gj_day_to_sec(uint64_t u64Day)
{
	(void)NULL;
	return b2535_mul_sec(u64Day);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_day_to_sec(uint64_t u64Day)
    __attribute__((alias("gj_day_to_sec")));
