/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3823: big-endian uint16 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_from_be_u(uint16_t x);
 *     - Interpret x as a big-endian wire word and return the host-order
 *       numerical value. Portable octet reverse (GreenJade is LE x86_64;
 *       matches ntohs semantics).
 *   uint16_t __gj_u16_from_be_u  (alias)
 *   __libcgj_batch3823_marker = "libcgj-batch3823"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_be16_u / gj_be16_load / htons/ntohs — pure integer from_be_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3823_marker[] = "libcgj-batch3823";

/* ---- freestanding helpers ---------------------------------------------- */

/* Portable 16-bit octet reverse (BE wire ↔ host on LE). */
static uint16_t
b3823_bswap16(uint16_t u16X)
{
	return (uint16_t)(((u16X & 0x00ffu) << 8) | ((u16X & 0xff00u) >> 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_from_be_u - convert a big-endian uint16 wire value to host order.
 *
 * x: value whose bit pattern is big-endian wire layout.
 * Returns host-order numerical value.
 */
uint16_t
gj_u16_from_be_u(uint16_t u16X)
{
	(void)NULL;
	return b3823_bswap16(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_from_be_u(uint16_t u16X)
    __attribute__((alias("gj_u16_from_be_u")));
