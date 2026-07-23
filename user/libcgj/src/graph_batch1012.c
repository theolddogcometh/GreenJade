/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1012: deposit low bits into a contiguous
 * bit field (simple PDEP-of-contiguous-mask).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_deposit_contiguous(uint64_t x, unsigned lo,
 *                                      unsigned len);
 *     — Take the low len bits of x and place them at bit positions
 *       [lo, lo+len). Other output bits are zero. lo >= 64 or len == 0
 *       → 0. len is clamped so the field does not extend past bit 63.
 *   uint64_t __gj_bit_deposit_contiguous  (alias)
 *   __libcgj_batch1012_marker = "libcgj-batch1012"
 *
 * Distinct from gj_bit_mask_between (batch1011) — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1012_marker[] = "libcgj-batch1012";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_deposit_contiguous — pack low len bits of x into [lo, lo+len).
 *
 * u64X: source value (only low len bits used)
 * lo:   destination starting bit index (0 = LSB)
 * len:  field width in bits
 *
 * Equivalent to: (x & ((1 << len) - 1)) << lo, with clamping and
 * shift-width safety. Output bits outside the field are zero.
 */
uint64_t
gj_bit_deposit_contiguous(uint64_t x, unsigned lo, unsigned len)
{
	unsigned uMaxLen;
	uint64_t uMask;

	if (len == 0u || lo >= 64u) {
		return 0ull;
	}

	uMaxLen = 64u - lo;
	if (len > uMaxLen) {
		len = uMaxLen;
	}

	if (len >= 64u) {
		/* lo == 0 and len == 64: deposit full word. */
		return x;
	}

	uMask = (((uint64_t)1) << len) - (uint64_t)1;
	return (x & uMask) << lo;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_deposit_contiguous(uint64_t x, unsigned lo, unsigned len)
    __attribute__((alias("gj_bit_deposit_contiguous")));
