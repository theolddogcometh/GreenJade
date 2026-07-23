/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1344: 2D Morton (Z-order) encode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_morton2_encode(uint16_t x, uint16_t y);
 *     — Encode 2D grid coordinates into a 32-bit Morton code
 *       (x in even bits, y in odd bits).
 *   uint32_t __gj_morton2_encode  (alias)
 *   __libcgj_batch1344_marker = "libcgj-batch1344"
 *
 * Distinct from gj_bit_interleave_u16 (batch1342) — named Morton surface.
 * Unique names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1344_marker[] = "libcgj-batch1344";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1344_part1by1(uint32_t u32X)
{
	u32X &= 0x0000ffffu;
	u32X = (u32X | (u32X << 8)) & 0x00ff00ffu;
	u32X = (u32X | (u32X << 4)) & 0x0f0f0f0fu;
	u32X = (u32X | (u32X << 2)) & 0x33333333u;
	u32X = (u32X | (u32X << 1)) & 0x55555555u;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_morton2_encode — 2D Z-order curve index from (x, y).
 *
 * u16X: X coordinate (even bit plane)
 * u16Y: Y coordinate (odd bit plane)
 *
 * Returns Morton code. Pure; all inputs accepted.
 */
uint32_t
gj_morton2_encode(uint16_t u16X, uint16_t u16Y)
{
	return b1344_part1by1((uint32_t)u16X) |
	    (b1344_part1by1((uint32_t)u16Y) << 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_morton2_encode(uint16_t u16X, uint16_t u16Y)
    __attribute__((alias("gj_morton2_encode")));
