/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3411: BLSMSK mask up to lowest set bit (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_blsmsk_u(uint64_t x);
 *     - Get mask up to and including the lowest set bit of x
 *       (BMI1 BLSMSK identity: x ^ (x - 1)).
 *       gj_u64_blsmsk_u(0) == UINT64_MAX (all bits set).
 *       gj_u64_blsmsk_u(1) == 1, gj_u64_blsmsk_u(8) == 0xf,
 *       gj_u64_blsmsk_u(0x12) == 0x3.
 *   uint64_t __gj_u64_blsmsk_u  (alias)
 *   __libcgj_batch3411_marker = "libcgj-batch3411"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u64_blsmsk_u
 * surface only; no multi-def. Distinct from gj_isolate_lsb (batch420)
 * which keeps only the LSB. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3411_marker[] = "libcgj-batch3411";

/* ---- freestanding helpers ---------------------------------------------- */

/* BLSMSK: bits from LSB through the lowest 1-bit inclusive. */
static uint64_t
b3411_blsmsk(uint64_t u64X)
{
	return u64X ^ (u64X - UINT64_C(1));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_blsmsk_u - mask up to lowest set bit of a 64-bit word.
 *
 * x: value to inspect
 *
 * Returns x ^ (x - 1). x == 0 yields all-ones (defined). No parent wires.
 */
uint64_t
gj_u64_blsmsk_u(uint64_t u64X)
{
	(void)NULL;
	return b3411_blsmsk(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_blsmsk_u(uint64_t u64X)
    __attribute__((alias("gj_u64_blsmsk_u")));
