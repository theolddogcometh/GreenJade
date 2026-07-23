/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3433: 64-bit byte (octet) swap (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_byte_swap_u(uint64_t x);
 *     - Reverse the eight octets of a 64-bit word
 *       (abcdefgh -> hgfedcba). Pure integer, no builtins.
 *   uint64_t __gj_u64_byte_swap_u  (alias)
 *   __libcgj_batch3433_marker = "libcgj-batch3433"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u64_byte_swap_u surface only; no multi-def. Distinct from
 * gj_bswap64 (batch417), gj_u64_byte_reverse (batch2136). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3433_marker[] = "libcgj-batch3433";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse eight 8-bit lanes of a 64-bit word. */
static uint64_t
b3433_byteswap(uint64_t u64X)
{
	return ((u64X & 0x00000000000000ffull) << 56) |
	       ((u64X & 0x000000000000ff00ull) << 40) |
	       ((u64X & 0x0000000000ff0000ull) << 24) |
	       ((u64X & 0x00000000ff000000ull) << 8) |
	       ((u64X & 0x000000ff00000000ull) >> 8) |
	       ((u64X & 0x0000ff0000000000ull) >> 24) |
	       ((u64X & 0x00ff000000000000ull) >> 40) |
	       ((u64X & 0xff00000000000000ull) >> 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_byte_swap_u - reverse octet order of a 64-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122334455667788 -> 0x8877665544332211.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_byte_swap_u(uint64_t u64X)
{
	(void)NULL;
	return b3433_byteswap(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_byte_swap_u(uint64_t u64X)
    __attribute__((alias("gj_u64_byte_swap_u")));
