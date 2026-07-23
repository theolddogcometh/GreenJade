/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2531: seconds to minutes (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_sec_to_min_floor(uint64_t sec);
 *     — Return floor(sec / 60).
 *   uint64_t __gj_sec_to_min_floor  (alias)
 *   __libcgj_batch2531_marker = "libcgj-batch2531"
 *
 * Time exclusive pure-data wave (2531–2540). Distinct from
 * gj_secs_to_hms (batch1437) field split — short-name floor surface.
 * Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2531_marker[] = "libcgj-batch2531";

/* Seconds per minute (SI: 1 min = 60 s). */
#define B2531_SEC_PER_MIN  60ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide seconds by minute size. */
static uint64_t
b2531_div_min(uint64_t u64Sec)
{
	return u64Sec / B2531_SEC_PER_MIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sec_to_min_floor — convert whole seconds to whole minutes (floor).
 *
 * sec: seconds
 * Returns floor(sec / 60).
 * Examples: 0 → 0, 59 → 0, 60 → 1, 125 → 2.
 */
uint64_t
gj_sec_to_min_floor(uint64_t u64Sec)
{
	(void)NULL;
	return b2531_div_min(u64Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sec_to_min_floor(uint64_t u64Sec)
    __attribute__((alias("gj_sec_to_min_floor")));
