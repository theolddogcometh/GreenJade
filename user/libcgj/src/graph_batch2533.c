/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2533: seconds to hours (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_sec_to_hour_floor(uint64_t sec);
 *     — Return floor(sec / 3600).
 *   uint64_t __gj_sec_to_hour_floor  (alias)
 *   __libcgj_batch2533_marker = "libcgj-batch2533"
 *
 * Time exclusive pure-data wave (2531–2540). Distinct from
 * gj_secs_to_hms (batch1437) — short-name floor surface. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2533_marker[] = "libcgj-batch2533";

/* Seconds per hour (SI: 1 h = 3600 s). */
#define B2533_SEC_PER_HOUR  3600ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide seconds by hour size. */
static uint64_t
b2533_div_hour(uint64_t u64Sec)
{
	return u64Sec / B2533_SEC_PER_HOUR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sec_to_hour_floor — convert whole seconds to whole hours (floor).
 *
 * sec: seconds
 * Returns floor(sec / 3600).
 * Examples: 0 → 0, 3599 → 0, 3600 → 1, 7201 → 2.
 */
uint64_t
gj_sec_to_hour_floor(uint64_t u64Sec)
{
	(void)NULL;
	return b2533_div_hour(u64Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sec_to_hour_floor(uint64_t u64Sec)
    __attribute__((alias("gj_sec_to_hour_floor")));
