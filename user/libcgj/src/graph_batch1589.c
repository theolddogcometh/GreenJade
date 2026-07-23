/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1589: nanoseconds to milliseconds (floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ns_to_ms_floor(uint64_t ns);
 *     — Return ns / 1000000 (floor division).
 *   uint64_t __gj_ns_to_ms_floor  (alias)
 *   __libcgj_batch1589_marker = "libcgj-batch1589"
 *
 * Inverse of gj_ms_to_ns (batch1586). Distinct from gj_ms_to_secs
 * (batch485) and gj_duration_ms (batch698). No multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1589_marker[] = "libcgj-batch1589";

/* Nanoseconds per millisecond (SI: 1 ms = 1e6 ns). */
#define B1589_NS_PER_MS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide nanoseconds by ms size. */
static uint64_t
b1589_div_ms(uint64_t u64Ns)
{
	return u64Ns / B1589_NS_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ns_to_ms_floor — convert nanoseconds to whole milliseconds (floor).
 *
 * u64Ns: nanoseconds
 * Returns floor(u64Ns / 1000000).
 * Examples: 0 → 0, 999999 → 0, 1000000 → 1, 2500000 → 2.
 */
uint64_t
gj_ns_to_ms_floor(uint64_t u64Ns)
{
	return b1589_div_ms(u64Ns);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ns_to_ms_floor(uint64_t u64Ns)
    __attribute__((alias("gj_ns_to_ms_floor")));
