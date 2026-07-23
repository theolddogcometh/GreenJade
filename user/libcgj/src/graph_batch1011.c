/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1011: contiguous bit mask over a half-open
 * bit index range [lo, hi).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_mask_between(unsigned lo, unsigned hi);
 *     — Return a mask with bits lo inclusive through hi exclusive set.
 *       lo >= hi → 0. Bits at index >= 64 are ignored (hi clamped at 64;
 *       lo >= 64 → 0). Example: lo=2, hi=5 → bits 2,3,4 → 0x1C.
 *   uint64_t __gj_bit_mask_between  (alias)
 *   __libcgj_batch1011_marker = "libcgj-batch1011"
 *
 * Distinct from gj_mask_low_u64 (batch592) — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1011_marker[] = "libcgj-batch1011";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_mask_between — bits [lo, hi) set in a uint64_t mask.
 *
 * lo: first bit index (0 = LSB), inclusive
 * hi: last bit index, exclusive
 *
 * Safe against shift-by-width UB: width == 0 → 0; width >= 64-lo with
 * lo == 0 and hi >= 64 → all ones via special case; otherwise
 * ((1 << width) - 1) << lo with width in 1..63.
 */
uint64_t
gj_bit_mask_between(unsigned lo, unsigned hi)
{
	unsigned uWidth;
	uint64_t uLow;

	if (lo >= hi || lo >= 64u) {
		return 0ull;
	}
	if (hi > 64u) {
		hi = 64u;
	}

	uWidth = hi - lo;
	if (uWidth >= 64u) {
		/* Only reachable when lo == 0 and hi == 64. */
		return ~(uint64_t)0;
	}

	uLow = (((uint64_t)1) << uWidth) - (uint64_t)1;
	return uLow << lo;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_mask_between(unsigned lo, unsigned hi)
    __attribute__((alias("gj_bit_mask_between")));
