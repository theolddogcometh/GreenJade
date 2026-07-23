/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4375: nanoseconds to microseconds floor (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ns_to_us_u(uint64_t ns);
 *     - Return floor(ns / 1000) whole microseconds.
 *   uint64_t __gj_ns_to_us_u  (alias)
 *   __libcgj_batch4375_marker = "libcgj-batch4375"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Distinct from
 * gj_us_to_ns (batch1587) — unique gj_ns_to_us_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4375_marker[] = "libcgj-batch4375";

/* Nanoseconds per microsecond (SI: 1 us = 1000 ns). */
#define B4375_NS_PER_US  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide nanoseconds by us size. */
static uint64_t
b4375_div_us(uint64_t u64Ns)
{
	return u64Ns / B4375_NS_PER_US;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ns_to_us_u - convert nanoseconds to whole microseconds (floor).
 *
 * ns: nanoseconds
 *
 * Returns floor(ns / 1000). Examples: 0 → 0, 999 → 0, 1000 → 1.
 * Self-contained; no parent wires.
 */
uint64_t
gj_ns_to_us_u(uint64_t u64Ns)
{
	(void)NULL;
	return b4375_div_us(u64Ns);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ns_to_us_u(uint64_t u64Ns)
    __attribute__((alias("gj_ns_to_us_u")));
