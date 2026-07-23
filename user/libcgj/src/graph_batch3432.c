/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3432: 32-bit byte (octet) swap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_byte_swap_u(uint32_t x);
 *     - Reverse the four octets of a 32-bit word (abcd -> dcba). Pure
 *       integer, no compiler builtins.
 *   uint32_t __gj_u32_byte_swap_u  (alias)
 *   __libcgj_batch3432_marker = "libcgj-batch3432"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u32_byte_swap_u surface only; no multi-def. Distinct from
 * gj_bswap32 (batch416), gj_bswap32_u (batch2746), gj_u32_byte_reverse
 * (batch2135). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3432_marker[] = "libcgj-batch3432";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse four 8-bit lanes of a 32-bit word. */
static uint32_t
b3432_byteswap(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_byte_swap_u - reverse octet order of a 32-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x11223344 -> 0x44332211. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_byte_swap_u(uint32_t u32X)
{
	(void)NULL;
	return b3432_byteswap(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_byte_swap_u(uint32_t u32X)
    __attribute__((alias("gj_u32_byte_swap_u")));
