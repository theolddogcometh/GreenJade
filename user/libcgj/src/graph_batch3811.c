/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3811: zero predicate for uint64_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_zero_u(uint64_t x);
 *     - Return 1 if x == 0, else 0.
 *   int __gj_u64_is_zero_u  (alias)
 *   __libcgj_batch3811_marker = "libcgj-batch3811"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820). Unique gj_u64_is_zero_u
 * surface only; no multi-def. Distinct from gj_mem_is_zero (batch607) /
 * gj_mac_is_zero / gj_ether_is_zero / gj_f64_bits_is_zero. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3811_marker[] = "libcgj-batch3811";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3811_is_zero(uint64_t u64X)
{
	return (u64X == 0ull) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_is_zero_u - 1 if x is zero, else 0.
 *
 * Pure integer compare. Does not call libc. No parent wires.
 */
int
gj_u64_is_zero_u(uint64_t u64X)
{
	(void)NULL;
	return b3811_is_zero(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_zero_u(uint64_t u64X)
    __attribute__((alias("gj_u64_is_zero_u")));
