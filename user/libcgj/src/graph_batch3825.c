/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3825: big-endian uint32 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_from_be_u(uint32_t x);
 *     - Interpret x as a big-endian wire word and return the host-order
 *       numerical value. Portable octet reverse (GreenJade is LE x86_64;
 *       matches ntohl semantics).
 *   uint32_t __gj_u32_from_be_u  (alias)
 *   __libcgj_batch3825_marker = "libcgj-batch3825"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_be32 / gj_bswap32 / htonl/ntohl — pure integer from_be_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3825_marker[] = "libcgj-batch3825";

/* ---- freestanding helpers ---------------------------------------------- */

/* Portable 32-bit octet reverse (BE wire ↔ host on LE). */
static uint32_t
b3825_bswap32(uint32_t u32X)
{
	return ((u32X & 0x000000ffu) << 24)
	    | ((u32X & 0x0000ff00u) << 8)
	    | ((u32X & 0x00ff0000u) >> 8)
	    | ((u32X & 0xff000000u) >> 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_from_be_u - convert a big-endian uint32 wire value to host order.
 *
 * x: value whose bit pattern is big-endian wire layout.
 * Returns host-order numerical value.
 */
uint32_t
gj_u32_from_be_u(uint32_t u32X)
{
	(void)NULL;
	return b3825_bswap32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_from_be_u(uint32_t u32X)
    __attribute__((alias("gj_u32_from_be_u")));
