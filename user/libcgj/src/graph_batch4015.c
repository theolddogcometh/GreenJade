/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4015: mask of bits strictly above a bit index
 * (uint64_t width).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mask_above_u(unsigned bit);
 *     - Return a mask with bits [bit+1 .. 63] set (strictly above bit).
 *       bit >= 63 -> 0 (nothing above bit 63, or out of range)
 *       else      -> ~((1ull << (bit+1)) - 1)  (safe: bit+1 in 1..63)
 *   uint64_t __gj_u64_mask_above_u  (alias)
 *   __libcgj_batch4015_marker = "libcgj-batch4015"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_mask_above_u (batch4014), gj_u64_mask_below_u (batch4013),
 * and gj_u64_mask_high_n (batch2016) — unique gj_u64_mask_above_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4015_marker[] = "libcgj-batch4015";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4015_mask_above(unsigned uBit)
{
	unsigned uNext;

	if (uBit >= 63u) {
		return 0ull;
	}
	/* Low (bit+1) bits clear; bits [bit+1 .. 63] set. */
	uNext = uBit + 1u;
	return ~(((uint64_t)1ull << uNext) - 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mask_above_u - mask with bits strictly above index bit set.
 *
 * bit: exclusive lower bound bit index (0 = LSB)
 *
 * Examples: bit=0 -> ~1ull; bit=62 -> 0x8000000000000000ULL;
 *           bit=63 -> 0; bit=100 -> 0. Safe against shift-by-width UB.
 * No parent wires.
 */
uint64_t
gj_u64_mask_above_u(unsigned uBit)
{
	(void)NULL;
	return b4015_mask_above(uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mask_above_u(unsigned uBit)
    __attribute__((alias("gj_u64_mask_above_u")));
