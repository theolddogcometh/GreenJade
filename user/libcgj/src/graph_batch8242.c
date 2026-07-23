/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8242: freestanding 32-bit octet swap (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bswap32_u_8242(uint32_t x);
 *     - Reverse the four octets of a 32-bit word (abcd -> dcba). Pure
 *       integer, no compiler builtins.
 *   uint32_t __gj_bswap32_u_8242  (alias)
 *   __libcgj_batch8242_marker = "libcgj-batch8242"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Distinct from gj_bswap32 (batch416), gj_bswap32_u (batch2746) —
 * unique gj_bswap32_u_8242 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8242_marker[] = "libcgj-batch8242";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8242_bswap32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24) |
	       ((u32X & 0x0000ff00u) << 8) |
	       ((u32X & 0x00ff0000u) >> 8) |
	       ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bswap32_u_8242 - reverse octet order of a 32-bit word.
 *
 * x: value whose octets are to be reversed
 *
 * Example: 0x11223344 -> 0x44332211.
 */
uint32_t
gj_bswap32_u_8242(uint32_t x)
{
	(void)NULL;
	return b8242_bswap32(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bswap32_u_8242(uint32_t x)
    __attribute__((alias("gj_bswap32_u_8242")));
