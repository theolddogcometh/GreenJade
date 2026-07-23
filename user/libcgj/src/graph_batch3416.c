/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3416: BEXTR bit-field extract (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bextr_u(uint32_t x, unsigned start, unsigned len);
 *     - BMI1 BEXTR: extract len consecutive bits starting at bit
 *       index start (0 = LSB). Result is right-aligned.
 *       len == 0 or start >= 32 yields 0. If start + len would run
 *       past bit 31, only the remaining high bits of x are taken
 *       (no undefined shift).
 *   uint32_t __gj_u32_bextr_u  (alias)
 *   __libcgj_batch3416_marker = "libcgj-batch3416"
 *
 * Exclusive continuum CREATE-ONLY (3411-3420). Unique gj_u32_bextr_u
 * surface only; no multi-def. Distinct from gj_u64_extract_bits_u
 * (batch3231) and gj_extract_bits_u64 (batch593). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3416_marker[] = "libcgj-batch3416";

/* ---- freestanding helpers ---------------------------------------------- */

/* Mask of the low uW bits of a u32. uW in 1..31. */
static uint32_t
b3416_mask_lo(unsigned uW)
{
	return (1u << uW) - 1u;
}

/* Right-aligned extract of [uStart, uStart+uLen) from u32X. */
static uint32_t
b3416_bextr(uint32_t u32X, unsigned uStart, unsigned uLen)
{
	unsigned uRem;

	if (uLen == 0u || uStart >= 32u) {
		return 0u;
	}

	uRem = 32u - uStart;
	if (uLen > uRem) {
		uLen = uRem;
	}

	u32X >>= uStart;
	if (uLen == 32u) {
		/* start was 0 and full width requested — whole word */
		return u32X;
	}

	return u32X & b3416_mask_lo(uLen);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bextr_u - right-aligned bitfield extract over [start, start+len).
 *
 * x:     source word
 * start: starting bit index (0 = LSB)
 * len:   number of bits to extract
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_bextr_u(uint32_t u32X, unsigned uStart, unsigned uLen)
{
	(void)NULL;
	return b3416_bextr(u32X, uStart, uLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bextr_u(uint32_t u32X, unsigned uStart, unsigned uLen)
    __attribute__((alias("gj_u32_bextr_u")));
