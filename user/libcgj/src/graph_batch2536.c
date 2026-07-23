/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2536: seconds to days (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_sec_to_day_floor(uint64_t sec);
 *     — Return floor(sec / 86400).
 *   uint64_t __gj_sec_to_day_floor  (alias)
 *   __libcgj_batch2536_marker = "libcgj-batch2536"
 *
 * Time exclusive pure-data wave (2531–2540). Inverse of
 * gj_day_to_sec (batch2535). Leap seconds ignored. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2536_marker[] = "libcgj-batch2536";

/* Seconds per day (SI: 1 d = 86400 s; no leap seconds). */
#define B2536_SEC_PER_DAY  86400ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide seconds by day size. */
static uint64_t
b2536_div_day(uint64_t u64Sec)
{
	return u64Sec / B2536_SEC_PER_DAY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sec_to_day_floor — convert whole seconds to whole days (floor).
 *
 * sec: seconds
 * Returns floor(sec / 86400).
 * Examples: 0 → 0, 86399 → 0, 86400 → 1, 172801 → 2.
 */
uint64_t
gj_sec_to_day_floor(uint64_t u64Sec)
{
	(void)NULL;
	return b2536_div_day(u64Sec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sec_to_day_floor(uint64_t u64Sec)
    __attribute__((alias("gj_sec_to_day_floor")));
