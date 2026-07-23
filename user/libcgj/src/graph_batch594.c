/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch594: deposit a bitfield into a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_deposit_bits_u64(uint64_t x, unsigned lo, unsigned len,
 *                                uint64_t v);
 *     — Replace the len bits of x starting at bit lo (LSB = bit 0)
 *       with the low len bits of v. Bits of x outside [lo, lo+len)
 *       are preserved. len == 0 leaves x unchanged. If the field would
 *       extend past bit 63, only the bits that fit (64 - lo) are written.
 *       lo >= 64 is a no-op (returns x).
 *   uint64_t __gj_deposit_bits_u64  (alias)
 *   __libcgj_batch594_marker = "libcgj-batch594"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch594_marker[] = "libcgj-batch594";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deposit_bits_u64 — bitfield insert into a 64-bit word.
 *
 * x:   host word
 * lo:  first bit index to replace (0 = LSB)
 * len: number of bits to replace
 * v:   source; only its low len bits (after clipping) are deposited
 *
 * mask = low-len ones; clear those bits in x at lo, OR in (v & mask) << lo.
 * Avoids shift-by-64 UB: len is clipped to at most 64 - lo, and the
 * all-ones path is used only when the (clipped) width is 64 (lo == 0).
 */
uint64_t
gj_deposit_bits_u64(uint64_t x, unsigned lo, unsigned len, uint64_t v)
{
	uint64_t u64Mask;
	unsigned uLen;

	if (len == 0u || lo >= 64u) {
		return x;
	}

	uLen = len;
	if (uLen > 64u - lo) {
		uLen = 64u - lo;
	}

	if (uLen == 64u) {
		return v;
	}

	u64Mask = (((uint64_t)1) << uLen) - (uint64_t)1;
	return (x & ~(u64Mask << lo)) | ((v & u64Mask) << lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_deposit_bits_u64(uint64_t x, unsigned lo, unsigned len,
    uint64_t v)
    __attribute__((alias("gj_deposit_bits_u64")));
