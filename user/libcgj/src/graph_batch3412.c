/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3412: TZMSK trailing-zero mask (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_tzmsk_u(uint32_t x);
 *     - Mask of trailing zero bits below the lowest set bit
 *       (AMD TBM TZMSK identity: (x - 1) & ~x).
 *       gj_u32_tzmsk_u(0) == 0xffffffffu (all bits set).
 *       gj_u32_tzmsk_u(1) == 0, gj_u32_tzmsk_u(8) == 7,
 *       gj_u32_tzmsk_u(0x12) == 1.
 *   uint32_t __gj_u32_tzmsk_u  (alias)
 *   __libcgj_batch3412_marker = "libcgj-batch3412"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u32_tzmsk_u
 * surface only; no multi-def. Distinct from gj_u32_ctz_u (batch2285)
 * which returns a count. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3412_marker[] = "libcgj-batch3412";

/* ---- freestanding helpers ---------------------------------------------- */

/* TZMSK: bits strictly below the lowest 1-bit (trailing zeros mask). */
static uint32_t
b3412_tzmsk(uint32_t u32X)
{
	return (u32X - 1u) & ~u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_tzmsk_u - trailing-zero bit mask of a 32-bit word.
 *
 * x: value to inspect
 *
 * Returns (x - 1) & ~x. x == 0 yields all-ones (defined). No parent wires.
 */
uint32_t
gj_u32_tzmsk_u(uint32_t u32X)
{
	(void)NULL;
	return b3412_tzmsk(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_tzmsk_u(uint32_t u32X)
    __attribute__((alias("gj_u32_tzmsk_u")));
