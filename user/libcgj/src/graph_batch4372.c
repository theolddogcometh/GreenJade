/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4372: microseconds to milliseconds floor (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_us_to_ms_u(uint64_t us);
 *     - Return floor(us / 1000) whole milliseconds.
 *   uint64_t __gj_us_to_ms_u  (alias)
 *   __libcgj_batch4372_marker = "libcgj-batch4372"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Inverse of
 * gj_ms_to_us_u (batch4371). Distinct from gj_us_to_ms_floor
 * (batch1972) and gj_time_us_to_ms (batch1034) — unique gj_us_to_ms_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4372_marker[] = "libcgj-batch4372";

/* Microseconds per millisecond (SI: 1 ms = 1000 us). */
#define B4372_US_PER_MS  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide microseconds by ms size. */
static uint64_t
b4372_div_ms(uint64_t u64Us)
{
	return u64Us / B4372_US_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_us_to_ms_u - convert microseconds to whole milliseconds (floor).
 *
 * us: microseconds
 *
 * Returns floor(us / 1000). Examples: 0 → 0, 999 → 0, 1000 → 1.
 * Self-contained; no parent wires.
 */
uint64_t
gj_us_to_ms_u(uint64_t u64Us)
{
	(void)NULL;
	return b4372_div_ms(u64Us);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_us_to_ms_u(uint64_t u64Us)
    __attribute__((alias("gj_us_to_ms_u")));
