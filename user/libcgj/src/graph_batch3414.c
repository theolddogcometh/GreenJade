/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3414: ANDN bitwise and-not (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_andn_u(uint32_t x, uint32_t y);
 *     - BMI1 ANDN: ~x & y (NOT of first operand AND second).
 *       gj_u32_andn_u(0, y) == y, gj_u32_andn_u(0xffffffffu, y) == 0,
 *       gj_u32_andn_u(0xf0f0f0f0u, 0xffffffffu) == 0x0f0f0f0fu.
 *   uint32_t __gj_u32_andn_u  (alias)
 *   __libcgj_batch3414_marker = "libcgj-batch3414"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u32_andn_u
 * surface only; no multi-def. Distinct from gj_bitmap_andnot_into
 * (batch1062) and gj_mem_andnot_bytes (batch2028). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3414_marker[] = "libcgj-batch3414";

/* ---- freestanding helpers ---------------------------------------------- */

/* ANDN: (~x) & y. */
static uint32_t
b3414_andn(uint32_t u32X, uint32_t u32Y)
{
	return (~u32X) & u32Y;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_andn_u - bitwise AND of inverted x with y (32-bit).
 *
 * x: value to invert
 * y: value to AND with ~x
 *
 * Returns (~x) & y. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_andn_u(uint32_t u32X, uint32_t u32Y)
{
	(void)NULL;
	return b3414_andn(u32X, u32Y);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_andn_u(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_u32_andn_u")));
