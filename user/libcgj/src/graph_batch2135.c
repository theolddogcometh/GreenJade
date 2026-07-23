/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2135: freestanding uint32 byte reverse (bswap32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_byte_reverse(uint32_t x);
 *     - Reverse the order of the four bytes in a 32-bit word.
 *       Pure integer; no builtins.
 *   uint32_t __gj_u32_byte_reverse  (alias)
 *   __libcgj_batch2135_marker = "libcgj-batch2135"
 *
 * Post-2130 bit reverse exclusive wave (2131-2140). Distinct from
 * bit-order reverses and any prior bswap surfaces - unique name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2135_marker[] = "libcgj-batch2135";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse four 8-bit lanes of a 32-bit word. */
static uint32_t
b2135_byterev(uint32_t u32X)
{
	uint32_t u32V;

	u32V = u32X;
	u32V = ((u32V & 0x000000ffu) << 24)
	     | ((u32V & 0x0000ff00u) << 8)
	     | ((u32V & 0x00ff0000u) >> 8)
	     | ((u32V & 0xff000000u) >> 24);
	return u32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_byte_reverse - reverse the four bytes of a 32-bit word.
 *
 * x: value whose bytes are to be reversed
 *
 * Example: 0x12345678 -> 0x78563412; 0x01020304 -> 0x04030201.
 */
uint32_t
gj_u32_byte_reverse(uint32_t u32X)
{
	(void)NULL;
	return b2135_byterev(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_byte_reverse(uint32_t u32X)
    __attribute__((alias("gj_u32_byte_reverse")));
