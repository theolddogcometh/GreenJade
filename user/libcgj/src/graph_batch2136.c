/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2136: freestanding uint64 byte reverse (bswap64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_byte_reverse(uint64_t x);
 *     - Reverse the order of the eight bytes in a 64-bit word.
 *       Pure integer; no builtins.
 *   uint64_t __gj_u64_byte_reverse  (alias)
 *   __libcgj_batch2136_marker = "libcgj-batch2136"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * gj_bswap64 (batch417) - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2136_marker[] = "libcgj-batch2136";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse eight 8-bit lanes of a 64-bit word (SWAR). */
static uint64_t
b2136_byterev(uint64_t u64X)
{
	uint64_t u64V;

	u64V = u64X;
	u64V = ((u64V & 0x00000000000000ffull) << 56)
	     | ((u64V & 0x000000000000ff00ull) << 40)
	     | ((u64V & 0x0000000000ff0000ull) << 24)
	     | ((u64V & 0x00000000ff000000ull) << 8)
	     | ((u64V & 0x000000ff00000000ull) >> 8)
	     | ((u64V & 0x0000ff0000000000ull) >> 24)
	     | ((u64V & 0x00ff000000000000ull) >> 40)
	     | ((u64V & 0xff00000000000000ull) >> 56);
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_byte_reverse - reverse the eight bytes of a 64-bit word.
 *
 * x: value whose bytes are to be reversed
 *
 * Example: 0x0102030405060708 -> 0x0807060504030201.
 */
uint64_t
gj_u64_byte_reverse(uint64_t u64X)
{
	(void)NULL;
	return b2136_byterev(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_byte_reverse(uint64_t u64X)
    __attribute__((alias("gj_u64_byte_reverse")));
