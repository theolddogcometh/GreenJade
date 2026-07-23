/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4014: mask of bits strictly above a bit index
 * (uint32_t width).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mask_above_u(unsigned bit);
 *     - Return a mask with bits [bit+1 .. 31] set (strictly above bit).
 *       bit >= 31 -> 0 (nothing above bit 31, or out of range)
 *       else      -> ~((1u << (bit+1)) - 1)  (safe: bit+1 in 1..31)
 *   uint32_t __gj_u32_mask_above_u  (alias)
 *   __libcgj_batch4014_marker = "libcgj-batch4014"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_mask_below_u (batch4012), gj_u64_mask_high_n (batch2016), and
 * gj_u32_mask_from_to_u (batch3568) — unique gj_u32_mask_above_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4014_marker[] = "libcgj-batch4014";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4014_mask_above(unsigned uBit)
{
	unsigned uNext;

	if (uBit >= 31u) {
		return 0u;
	}
	/* Low (bit+1) bits clear; bits [bit+1 .. 31] set. */
	uNext = uBit + 1u;
	return ~(((uint32_t)1u << uNext) - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mask_above_u - mask with bits strictly above index bit set.
 *
 * bit: exclusive lower bound bit index (0 = LSB)
 *
 * Examples: bit=0 -> 0xFFFFFFFE; bit=30 -> 0x80000000; bit=31 -> 0;
 *           bit=100 -> 0. Safe against shift-by-width UB.
 * No parent wires.
 */
uint32_t
gj_u32_mask_above_u(unsigned uBit)
{
	(void)NULL;
	return b4014_mask_above(uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mask_above_u(unsigned uBit)
    __attribute__((alias("gj_u32_mask_above_u")));
