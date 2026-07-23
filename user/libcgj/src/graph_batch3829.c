/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3829: host uint16 to big-endian wire value (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_to_be_u(uint16_t x);
 *     - Convert host-order numerical value x to a big-endian wire word.
 *       Portable octet reverse (GreenJade is LE x86_64; matches htons).
 *   uint16_t __gj_u16_to_be_u  (alias)
 *   __libcgj_batch3829_marker = "libcgj-batch3829"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_store_be16_u / gj_u16_from_be_u / htons — pure integer to_be_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3829_marker[] = "libcgj-batch3829";

/* ---- freestanding helpers ---------------------------------------------- */

/* Portable 16-bit octet reverse (host ↔ BE wire on LE). */
static uint16_t
b3829_bswap16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_to_be_u - convert a host-order uint16 to a big-endian wire value.
 *
 * x: host-order numerical value.
 * Returns big-endian wire bit pattern.
 */
uint16_t
gj_u16_to_be_u(uint16_t u16X)
{
	(void)NULL;
	return b3829_bswap16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_to_be_u(uint16_t u16X)
    __attribute__((alias("gj_u16_to_be_u")));
