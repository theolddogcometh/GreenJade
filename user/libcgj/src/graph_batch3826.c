/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3826: little-endian uint32 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_from_le_u(uint32_t x);
 *     - Interpret x as a little-endian wire word and return the host-order
 *       numerical value. On GreenJade LE x86_64 this is the identity.
 *   uint32_t __gj_u32_from_le_u  (alias)
 *   __libcgj_batch3826_marker = "libcgj-batch3826"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_le32 / gj_u32_from_be_u — pure integer from_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3826_marker[] = "libcgj-batch3826";

/* ---- freestanding helpers ---------------------------------------------- */

/* LE wire → host on LE: identity. */
static uint32_t
b3826_from_le(uint32_t u32X)
{
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_from_le_u - convert a little-endian uint32 wire value to host order.
 *
 * x: value whose bit pattern is little-endian wire layout.
 * Returns host-order numerical value (identity on LE hosts).
 */
uint32_t
gj_u32_from_le_u(uint32_t u32X)
{
	(void)NULL;
	return b3826_from_le(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_from_le_u(uint32_t u32X)
    __attribute__((alias("gj_u32_from_le_u")));
