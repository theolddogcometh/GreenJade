/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3824: little-endian uint16 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_from_le_u(uint16_t x);
 *     - Interpret x as a little-endian wire word and return the host-order
 *       numerical value. On GreenJade LE x86_64 this is the identity.
 *   uint16_t __gj_u16_from_le_u  (alias)
 *   __libcgj_batch3824_marker = "libcgj-batch3824"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_le16_u / gj_le16_load / gj_u16_from_be_u — pure integer
 * from_le_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3824_marker[] = "libcgj-batch3824";

/* ---- freestanding helpers ---------------------------------------------- */

/* LE wire → host on LE: identity. */
static uint16_t
b3824_from_le(uint16_t u16X)
{
	return u16X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_from_le_u - convert a little-endian uint16 wire value to host order.
 *
 * x: value whose bit pattern is little-endian wire layout.
 * Returns host-order numerical value (identity on LE hosts).
 */
uint16_t
gj_u16_from_le_u(uint16_t u16X)
{
	(void)NULL;
	return b3824_from_le(u16X);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_from_le_u(uint16_t u16X)
    __attribute__((alias("gj_u16_from_le_u")));
