/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3179: full predicate for a uint64_t bitset word.
 *
 * Surface (unique symbols):
 *   int gj_bitset64_full_p_u(uint64_t x);
 *     - Return 1 if all 64 bits of x are set (x == UINT64_MAX), else 0.
 *   int __gj_bitset64_full_p_u  (alias)
 *   __libcgj_batch3179_marker = "libcgj-batch3179"
 *
 * Milestone 3180 exclusive continuum CREATE-ONLY (3171-3180). Unique
 * gj_bitset64_full_p_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3179_marker[] = "libcgj-batch3179";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff every bit is set. */
static int
b3179_full_p(uint64_t u64X)
{
	return u64X == UINT64_MAX ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset64_full_p_u - predicate: is the 64-bit bitset word full?
 *
 * x: 64-bit bitset word
 *
 * Returns 1 when x == UINT64_MAX (all bits set), else 0. No parent wires.
 */
int
gj_bitset64_full_p_u(uint64_t u64X)
{
	(void)NULL;
	return b3179_full_p(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset64_full_p_u(uint64_t u64X)
    __attribute__((alias("gj_bitset64_full_p_u")));
