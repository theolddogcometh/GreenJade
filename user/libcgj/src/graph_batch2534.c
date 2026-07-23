/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2534: hours to seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hour_to_sec(uint64_t hour);
 *     — Return hour * 3600, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_hour_to_sec  (alias)
 *   __libcgj_batch2534_marker = "libcgj-batch2534"
 *
 * Time exclusive pure-data wave (2531–2540). Inverse of
 * gj_sec_to_hour_floor (batch2533). Distinct from gj_hms_to_secs
 * (batch1438). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2534_marker[] = "libcgj-batch2534";

/* Seconds per hour (SI: 1 h = 3600 s). */
#define B2534_SEC_PER_HOUR  3600ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by B2534_SEC_PER_HOUR. */
static uint64_t
b2534_mul_sec(uint64_t u64Hour)
{
	if (u64Hour > (UINT64_MAX / B2534_SEC_PER_HOUR)) {
		return UINT64_MAX;
	}
	return u64Hour * B2534_SEC_PER_HOUR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hour_to_sec — convert whole hours to seconds (saturating).
 *
 * hour: hours (duration-style; unrestricted)
 * Returns hour * 3600, or UINT64_MAX if the product would overflow.
 * Examples: 0 → 0, 1 → 3600, 2 → 7200.
 */
uint64_t
gj_hour_to_sec(uint64_t u64Hour)
{
	(void)NULL;
	return b2534_mul_sec(u64Hour);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hour_to_sec(uint64_t u64Hour)
    __attribute__((alias("gj_hour_to_sec")));
