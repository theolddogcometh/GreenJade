/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8346: nanoseconds to milliseconds floor (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ns_to_ms_u_8346(uint64_t ns);
 *     - Return floor(ns / 1000000) whole milliseconds.
 *   uint64_t __gj_ns_to_ms_u_8346  (alias)
 *   __libcgj_batch8346_marker = "libcgj-batch8346"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Inverse of gj_ms_to_ns_u_8344 (batch8344). Distinct from
 * gj_ns_to_ms_floor (batch1589) — unique gj_ns_to_ms_u_8346 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8346_marker[] = "libcgj-batch8346";

/* Nanoseconds per millisecond (SI: 1 ms = 1e6 ns). */
#define B8346_NS_PER_MS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b8346_ns_to_ms(uint64_t u64Ns)
{
	return u64Ns / B8346_NS_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ns_to_ms_u_8346 - convert nanoseconds to whole milliseconds (floor).
 *
 * ns: nanoseconds
 *
 * Returns floor(ns / 1000000). Examples: 0 → 0, 999999 → 0, 1000000 → 1.
 * Soft pure-integer unit stub; no parent wires.
 */
uint64_t
gj_ns_to_ms_u_8346(uint64_t ns)
{
	(void)NULL;
	return b8346_ns_to_ms(ns);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ns_to_ms_u_8346(uint64_t ns)
    __attribute__((alias("gj_ns_to_ms_u_8346")));
