/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1013: extract a contiguous bit field to the
 * low bits (simple PEXT-of-contiguous-mask).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bit_extract_contiguous(uint64_t x, unsigned lo,
 *                                      unsigned len);
 *     — Extract bits [lo, lo+len) of x and return them packed into the
 *       low len bits of the result. lo >= 64 or len == 0 → 0. len is
 *       clamped so the field does not extend past bit 63.
 *   uint64_t __gj_bit_extract_contiguous  (alias)
 *   __libcgj_batch1013_marker = "libcgj-batch1013"
 *
 * Distinct from gj_bit_deposit_contiguous (batch1012) — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1013_marker[] = "libcgj-batch1013";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_extract_contiguous — (x >> lo) masked to len bits.
 *
 * u64X: source value
 * lo:   starting bit index of the field (0 = LSB)
 * len:  field width in bits
 *
 * Equivalent to: (x >> lo) & ((1 << len) - 1), with clamping and
 * shift-width safety.
 */
uint64_t
gj_bit_extract_contiguous(uint64_t x, unsigned lo, unsigned len)
{
	unsigned uMaxLen;
	uint64_t uShifted;
	uint64_t uMask;

	if (len == 0u || lo >= 64u) {
		return 0ull;
	}

	uMaxLen = 64u - lo;
	if (len > uMaxLen) {
		len = uMaxLen;
	}

	uShifted = x >> lo;
	if (len >= 64u) {
		return uShifted;
	}

	uMask = (((uint64_t)1) << len) - (uint64_t)1;
	return uShifted & uMask;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bit_extract_contiguous(uint64_t x, unsigned lo, unsigned len)
    __attribute__((alias("gj_bit_extract_contiguous")));
