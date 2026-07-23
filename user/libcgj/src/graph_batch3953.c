/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3953: freestanding uint32 byte reverse (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_byte_reverse_u(uint32_t x);
 *     - Reverse the order of the four bytes in a 32-bit word.
 *       Pure integer; no builtins.
 *   uint32_t __gj_u32_byte_reverse_u  (alias)
 *   __libcgj_batch3953_marker = "libcgj-batch3953"
 *
 * CREATE-ONLY exclusive continuum wave (3951-3960). Unique
 * gj_u32_byte_reverse_u surface only; no multi-def. Distinct from
 * gj_u32_byte_reverse (batch2135), gj_u32_byte_swap_u (batch3432),
 * gj_bswap32 (batch416). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3953_marker[] = "libcgj-batch3953";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse four 8-bit lanes of a 32-bit word. */
static uint32_t
b3953_byterev(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24)
	     | ((u32X & 0x0000ff00u) << 8)
	     | ((u32X & 0x00ff0000u) >> 8)
	     | ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_byte_reverse_u - reverse the four bytes of a 32-bit word.
 *
 * x: value whose bytes are to be reversed
 *
 * Example: 0x12345678 -> 0x78563412; 0x01020304 -> 0x04030201.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_byte_reverse_u(uint32_t u32X)
{
	(void)NULL;
	return b3953_byterev(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_byte_reverse_u(uint32_t u32X)
    __attribute__((alias("gj_u32_byte_reverse_u")));
