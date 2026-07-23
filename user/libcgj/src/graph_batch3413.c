/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3413: TZMSK trailing-zero mask (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_tzmsk_u(uint64_t x);
 *     - Mask of trailing zero bits below the lowest set bit
 *       (AMD TBM TZMSK identity: (x - 1) & ~x).
 *       gj_u64_tzmsk_u(0) == UINT64_MAX (all bits set).
 *       gj_u64_tzmsk_u(1) == 0, gj_u64_tzmsk_u(8) == 7,
 *       gj_u64_tzmsk_u(0x12) == 1.
 *   uint64_t __gj_u64_tzmsk_u  (alias)
 *   __libcgj_batch3413_marker = "libcgj-batch3413"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u64_tzmsk_u
 * surface only; no multi-def. Distinct from gj_u32_tzmsk_u (batch3412)
 * and gj_u64_ctz_u (batch2286). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3413_marker[] = "libcgj-batch3413";

/* ---- freestanding helpers ---------------------------------------------- */

/* TZMSK: bits strictly below the lowest 1-bit (trailing zeros mask). */
static uint64_t
b3413_tzmsk(uint64_t u64X)
{
	return (u64X - UINT64_C(1)) & ~u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_tzmsk_u - trailing-zero bit mask of a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns (x - 1) & ~x. x == 0 yields all-ones (defined). No parent wires.
 */
uint64_t
gj_u64_tzmsk_u(uint64_t u64X)
{
	(void)NULL;
	return b3413_tzmsk(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_tzmsk_u(uint64_t u64X)
    __attribute__((alias("gj_u64_tzmsk_u")));
