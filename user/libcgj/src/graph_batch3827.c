/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3827: big-endian uint64 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_from_be_u(uint64_t x);
 *     - Interpret x as a big-endian wire word and return the host-order
 *       numerical value. Portable octet reverse (GreenJade is LE x86_64).
 *   uint64_t __gj_u64_from_be_u  (alias)
 *   __libcgj_batch3827_marker = "libcgj-batch3827"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_be64 / gj_bswap64 / gj_u64_byte_reverse — pure integer
 * from_be_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3827_marker[] = "libcgj-batch3827";

/* ---- freestanding helpers ---------------------------------------------- */

/* Portable 64-bit octet reverse (BE wire ↔ host on LE). */
static uint64_t
b3827_bswap64(uint64_t u64X)
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
 * gj_u64_from_be_u - convert a big-endian uint64 wire value to host order.
 *
 * x: value whose bit pattern is big-endian wire layout.
 * Returns host-order numerical value.
 */
uint64_t
gj_u64_from_be_u(uint64_t u64X)
{
	(void)NULL;
	return b3827_bswap64(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_from_be_u(uint64_t u64X)
    __attribute__((alias("gj_u64_from_be_u")));
