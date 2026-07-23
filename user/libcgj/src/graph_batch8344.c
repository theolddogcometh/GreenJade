/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8344: milliseconds to nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_ns_u_8344(uint64_t ms);
 *     - Return ms * 1000000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_ms_to_ns_u_8344  (alias)
 *   __libcgj_batch8344_marker = "libcgj-batch8344"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Distinct from gj_ms_to_ns (batch1586) — unique gj_ms_to_ns_u_8344
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8344_marker[] = "libcgj-batch8344";

/* Nanoseconds per millisecond (SI: 1 ms = 1e6 ns). */
#define B8344_NS_PER_MS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b8344_ms_to_ns(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / B8344_NS_PER_MS)) {
		return UINT64_MAX;
	}
	return u64Ms * B8344_NS_PER_MS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_ns_u_8344 - convert milliseconds to nanoseconds (saturating).
 *
 * ms: milliseconds
 *
 * Returns ms * 1000000, or UINT64_MAX if the product would overflow.
 * Soft pure-integer unit stub; no parent wires.
 */
uint64_t
gj_ms_to_ns_u_8344(uint64_t ms)
{
	(void)NULL;
	return b8344_ms_to_ns(ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_ns_u_8344(uint64_t ms)
    __attribute__((alias("gj_ms_to_ns_u_8344")));
