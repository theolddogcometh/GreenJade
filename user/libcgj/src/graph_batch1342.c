/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1342: bit-interleave two u16 into one u32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_interleave_u16(uint16_t a, uint16_t b);
 *     — Spread bits of a into even positions and bits of b into odd
 *       positions of a 32-bit result (classic 2-way bit zip).
 *   uint32_t __gj_bit_interleave_u16  (alias)
 *   __libcgj_batch1342_marker = "libcgj-batch1342"
 *
 * Distinct from gj_morton2_encode (batch1344). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1342_marker[] = "libcgj-batch1342";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Spread the low 16 bits of u32X into the even bit positions of a u32
 * (bit i of input → bit 2i of output). Magic-mask expand.
 */
static uint32_t
b1342_part1by1(uint32_t u32X)
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
 * gj_bit_interleave_u16 — zip bits of two 16-bit words into 32 bits.
 *
 * u16A: even-bit source (result bits 0,2,4,...)
 * u16B: odd-bit source  (result bits 1,3,5,...)
 *
 * Returns the interleaved 32-bit word. Pure; all inputs accepted.
 */
uint32_t
gj_bit_interleave_u16(uint16_t u16A, uint16_t u16B)
{
	return b1342_part1by1((uint32_t)u16A) |
	    (b1342_part1by1((uint32_t)u16B) << 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_interleave_u16(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_bit_interleave_u16")));
