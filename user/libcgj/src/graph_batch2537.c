/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2537: weeks to seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_week_to_sec(uint64_t week);
 *     — Return week * 604800, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_week_to_sec  (alias)
 *   __libcgj_batch2537_marker = "libcgj-batch2537"
 *
 * Time exclusive pure-data wave (2531–2540). Week = 7 * 86400 SI
 * seconds (no leap seconds). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2537_marker[] = "libcgj-batch2537";

/* Seconds per week (7 * 86400 = 604800; no leap seconds). */
#define B2537_SEC_PER_WEEK  604800ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B2537_SEC_PER_WEEK. */
static uint64_t
b2537_mul_sec(uint64_t u64Week)
{
	if (u64Week > (UINT64_MAX / B2537_SEC_PER_WEEK)) {
		return UINT64_MAX;
	}
	return u64Week * B2537_SEC_PER_WEEK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_week_to_sec — convert whole weeks to seconds (saturating).
 *
 * week: weeks
 * Returns week * 604800, or UINT64_MAX if the product would overflow.
 * Examples: 0 → 0, 1 → 604800, 2 → 1209600.
 */
uint64_t
gj_week_to_sec(uint64_t u64Week)
{
	(void)NULL;
	return b2537_mul_sec(u64Week);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_week_to_sec(uint64_t u64Week)
    __attribute__((alias("gj_week_to_sec")));
