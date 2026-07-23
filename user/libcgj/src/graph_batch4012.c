/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4012: mask of bits strictly below a bit index
 * (uint32_t width).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mask_below_u(unsigned bit);
 *     - Return a mask with bits [0 .. bit-1] set (strictly below bit).
 *       bit == 0  -> 0
 *       bit >= 32 -> all ones (0xFFFFFFFF)
 *       else      -> (1u << bit) - 1  (safe: no shift-by-width UB)
 *   uint32_t __gj_u32_mask_below_u  (alias)
 *   __libcgj_batch4012_marker = "libcgj-batch4012"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_mask_low_u64 (batch592), gj_u64_mask_low_n (batch2015), and
 * gj_u32_mask_from_to_u (batch3568) — unique gj_u32_mask_below_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4012_marker[] = "libcgj-batch4012";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4012_mask_below(unsigned uBit)
{
	if (uBit >= 32u) {
		return ~(uint32_t)0u;
	}
	return ((uint32_t)1u << uBit) - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mask_below_u - mask with bits strictly below index bit set.
 *
 * bit: exclusive upper bound bit index (0 = LSB side)
 *
 * Examples: bit=0 -> 0; bit=1 -> 0x1; bit=4 -> 0xF; bit=32 -> all ones;
 *           bit=100 -> all ones. Safe against shift-by-width UB.
 * No parent wires.
 */
uint32_t
gj_u32_mask_below_u(unsigned uBit)
{
	(void)NULL;
	return b4012_mask_below(uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mask_below_u(unsigned uBit)
    __attribute__((alias("gj_u32_mask_below_u")));
