/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4013: mask of bits strictly below a bit index
 * (uint64_t width).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mask_below_u(unsigned bit);
 *     - Return a mask with bits [0 .. bit-1] set (strictly below bit).
 *       bit == 0  -> 0
 *       bit >= 64 -> all ones (0xFFFFFFFFFFFFFFFF)
 *       else      -> (1ull << bit) - 1  (safe: no shift-by-width UB)
 *   uint64_t __gj_u64_mask_below_u  (alias)
 *   __libcgj_batch4013_marker = "libcgj-batch4013"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_mask_below_u (batch4012), gj_mask_low_u64 (batch592), and
 * gj_u64_mask_low_n (batch2015) — unique gj_u64_mask_below_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4013_marker[] = "libcgj-batch4013";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4013_mask_below(unsigned uBit)
{
	if (uBit >= 64u) {
		return ~(uint64_t)0ull;
	}
	return ((uint64_t)1ull << uBit) - 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mask_below_u - mask with bits strictly below index bit set.
 *
 * bit: exclusive upper bound bit index (0 = LSB side)
 *
 * Examples: bit=0 -> 0; bit=1 -> 0x1; bit=8 -> 0xFF; bit=64 -> all ones.
 * Safe against shift-by-width UB. No parent wires.
 */
uint64_t
gj_u64_mask_below_u(unsigned uBit)
{
	(void)NULL;
	return b4013_mask_below(uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mask_below_u(unsigned uBit)
    __attribute__((alias("gj_u64_mask_below_u")));
