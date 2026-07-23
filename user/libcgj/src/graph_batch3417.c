/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3417: BEXTR bit-field extract (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bextr_u(uint64_t x, unsigned start, unsigned len);
 *     - BMI1 BEXTR: extract len consecutive bits starting at bit
 *       index start (0 = LSB). Result is right-aligned.
 *       len == 0 or start >= 64 yields 0. If start + len would run
 *       past bit 63, only the remaining high bits of x are taken
 *       (no undefined shift).
 *   uint64_t __gj_u64_bextr_u  (alias)
 *   __libcgj_batch3417_marker = "libcgj-batch3417"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u64_bextr_u
 * surface only; no multi-def. Distinct from gj_u32_bextr_u (batch3416),
 * gj_u64_extract_bits_u (batch3231), and gj_extract_bits_u64 (batch593).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3417_marker[] = "libcgj-batch3417";

/* ---- freestanding helpers ---------------------------------------------- */

/* Mask of the low uW bits of a u64. uW in 1..63. */
static uint64_t
b3417_mask_lo(unsigned uW)
{
	return (UINT64_C(1) << uW) - UINT64_C(1);
}

/* Right-aligned extract of [uStart, uStart+uLen) from u64X. */
static uint64_t
b3417_bextr(uint64_t u64X, unsigned uStart, unsigned uLen)
{
	unsigned uRem;

	if (uLen == 0u || uStart >= 64u) {
		return UINT64_C(0);
	}

	uRem = 64u - uStart;
	if (uLen > uRem) {
		uLen = uRem;
	}

	u64X >>= uStart;
	if (uLen == 64u) {
		/* start was 0 and full width requested — whole word */
		return u64X;
	}

	return u64X & b3417_mask_lo(uLen);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bextr_u - right-aligned bitfield extract over [start, start+len).
 *
 * x:     source word
 * start: starting bit index (0 = LSB)
 * len:   number of bits to extract
 *
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_bextr_u(uint64_t u64X, unsigned uStart, unsigned uLen)
{
	(void)NULL;
	return b3417_bextr(u64X, uStart, uLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bextr_u(uint64_t u64X, unsigned uStart, unsigned uLen)
    __attribute__((alias("gj_u64_bextr_u")));
