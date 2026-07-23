/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3954: freestanding uint64 byte reverse (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_byte_reverse_u(uint64_t x);
 *     - Reverse the order of the eight bytes in a 64-bit word.
 *       Pure integer; no builtins.
 *   uint64_t __gj_u64_byte_reverse_u  (alias)
 *   __libcgj_batch3954_marker = "libcgj-batch3954"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u64_byte_reverse_u surface only; no multi-def. Distinct from
 * gj_u64_byte_reverse (batch2136) and gj_bswap64 (batch417).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3954_marker[] = "libcgj-batch3954";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse eight 8-bit lanes of a 64-bit word (SWAR). */
static uint64_t
b3954_byterev(uint64_t u64X)
{
	return ((u64X & 0x00000000000000ffull) << 56)
	     | ((u64X & 0x000000000000ff00ull) << 40)
	     | ((u64X & 0x0000000000ff0000ull) << 24)
	     | ((u64X & 0x00000000ff000000ull) << 8)
	     | ((u64X & 0x000000ff00000000ull) >> 8)
	     | ((u64X & 0x0000ff0000000000ull) >> 24)
	     | ((u64X & 0x00ff000000000000ull) >> 40)
	     | ((u64X & 0xff00000000000000ull) >> 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_byte_reverse_u - reverse the eight bytes of a 64-bit word.
 *
 * x: value whose bytes are to be reversed
 *
 * Example: 0x0102030405060708 -> 0x0807060504030201.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_byte_reverse_u(uint64_t u64X)
{
	(void)NULL;
	return b3954_byterev(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_byte_reverse_u(uint64_t u64X)
    __attribute__((alias("gj_u64_byte_reverse_u")));
