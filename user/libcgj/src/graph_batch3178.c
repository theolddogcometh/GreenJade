/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3178: empty predicate for a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   int gj_bitset64_empty_p_u(uint64_t x);
 *     - Return 1 if x has no bits set (x == 0), else 0.
 *   int __gj_bitset64_empty_p_u  (alias)
 *   __libcgj_batch3178_marker = "libcgj-batch3178"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Unique
 * gj_bitset64_empty_p_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3178_marker[] = "libcgj-batch3178";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff no bits set. */
static int
b3178_empty_p(uint64_t u64X)
{
	return u64X == 0ull ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_empty_p_u - predicate: is the 64-bit bitset word empty?
 *
 * x: 64-bit bitset word
 *
 * Returns 1 when x == 0 (no bits set), else 0. No parent wires.
 */
int
gj_bitset64_empty_p_u(uint64_t u64X)
{
	(void)NULL;
	return b3178_empty_p(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_empty_p_u(uint64_t u64X)
    __attribute__((alias("gj_bitset64_empty_p_u")));
