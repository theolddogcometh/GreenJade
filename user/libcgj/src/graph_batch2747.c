/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2747: 64-bit octet swap (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bswap64_u(uint64_t x);
 *     - Reverse the eight octets of a 64-bit word
 *       (abcdefgh -> hgfedcba). Pure integer, no compiler builtins.
 *   uint64_t __gj_bswap64_u  (alias)
 *   __libcgj_batch2747_marker = "libcgj-batch2747"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_bswap64
 * (batch417), gj_bswap_u64 (batch775), and gj_byteswap64 (batch559) —
 * unique gj_bswap64_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2747_marker[] = "libcgj-batch2747";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2747_bswap64(uint64_t u64X)
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
 * gj_bswap64_u - reverse octet order of a 64-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x1122334455667788 -> 0x8877665544332211.
 */
uint64_t
gj_bswap64_u(uint64_t x)
{
	(void)NULL;
	return b2747_bswap64(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bswap64_u(uint64_t x)
    __attribute__((alias("gj_bswap64_u")));
