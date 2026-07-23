/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1972: microseconds to milliseconds (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_us_to_ms_floor(uint64_t us);
 *     — Return us / 1000 (floor division).
 *   uint64_t __gj_us_to_ms_floor  (alias)
 *   __libcgj_batch1972_marker = "libcgj-batch1972"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Inverse of
 * gj_ms_to_us (batch1971). Distinct from gj_time_us_to_ms (batch1034).
 * Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1972_marker[] = "libcgj-batch1972";

/* Microseconds per millisecond (SI: 1 ms = 1000 us). */
#define B1972_US_PER_MS  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide microseconds by ms size. */
static uint64_t
b1972_div_ms(uint64_t u64Us)
{
	return u64Us / B1972_US_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_us_to_ms_floor — convert microseconds to whole milliseconds (floor).
 *
 * us: microseconds
 * Returns floor(us / 1000).
 * Examples: 0 → 0, 999 → 0, 1000 → 1, 2500 → 2.
 */
uint64_t
gj_us_to_ms_floor(uint64_t u64Us)
{
	(void)NULL;
	return b1972_div_ms(u64Us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_us_to_ms_floor(uint64_t u64Us)
    __attribute__((alias("gj_us_to_ms_floor")));
