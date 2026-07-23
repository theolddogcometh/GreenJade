/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3415: ANDN bitwise and-not (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_andn_u(uint64_t x, uint64_t y);
 *     - BMI1 ANDN: ~x & y (NOT of first operand AND second).
 *       gj_u64_andn_u(0, y) == y, gj_u64_andn_u(UINT64_MAX, y) == 0,
 *       gj_u64_andn_u(0xf0f0f0f0f0f0f0f0ull, UINT64_MAX) ==
 *         0x0f0f0f0f0f0f0f0full.
 *   uint64_t __gj_u64_andn_u  (alias)
 *   __libcgj_batch3415_marker = "libcgj-batch3415"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u64_andn_u
 * surface only; no multi-def. Distinct from gj_u32_andn_u (batch3414)
 * and gj_bitmap_andnot_into (batch1062). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3415_marker[] = "libcgj-batch3415";

/* ---- freestanding helpers ---------------------------------------------- */

/* ANDN: (~x) & y. */
static uint64_t
b3415_andn(uint64_t u64X, uint64_t u64Y)
{
	return (~u64X) & u64Y;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_andn_u - bitwise AND of inverted x with y (64-bit).
 *
 * x: value to invert
 * y: value to AND with ~x
 *
 * Returns (~x) & y. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_andn_u(uint64_t u64X, uint64_t u64Y)
{
	(void)NULL;
	return b3415_andn(u64X, u64Y);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_andn_u(uint64_t u64X, uint64_t u64Y)
    __attribute__((alias("gj_u64_andn_u")));
