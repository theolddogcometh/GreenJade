/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3231: extract a bitfield from a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_extract_bits_u(uint64_t x, unsigned lo, unsigned len);
 *     - Extract len consecutive bits starting at bit index lo (0 = LSB).
 *       Result is right-aligned in the low bits of the return value.
 *       len == 0 or lo >= 64 yields 0. If lo + len would run past bit 63,
 *       only the remaining high bits of x are taken (no undefined shift).
 *   uint64_t __gj_u64_extract_bits_u  (alias)
 *   __libcgj_batch3231_marker = "libcgj-batch3231"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u64_extract_bits_u surface only; no multi-def. Distinct from
 * gj_extract_bits_u64 (batch593) and gj_u64_extract_bits (batch1389).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3231_marker[] = "libcgj-batch3231";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Mask of the low uW bits of a u64. uW must be in 1..63; caller handles
 * the full-width (64) and empty (0) cases separately to avoid (1<<64).
 */
static uint64_t
b3231_mask_lo(unsigned uW)
{
	return (UINT64_C(1) << uW) - UINT64_C(1);
}

/* Right-aligned extract of [uLo, uLo+uLen) from u64X. */
static uint64_t
b3231_extract(uint64_t u64X, unsigned uLo, unsigned uLen)
{
	unsigned uRem;

	if (uLen == 0u || uLo >= 64u) {
		return UINT64_C(0);
	}

	uRem = 64u - uLo;
	if (uLen > uRem) {
		uLen = uRem;
	}

	u64X >>= uLo;
	if (uLen == 64u) {
		/* lo was 0 and full width requested — whole word */
		return u64X;
	}

	return u64X & b3231_mask_lo(uLen);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_extract_bits_u - right-aligned bitfield extract over [lo, lo+len).
 *
 * x:   source word
 * lo:  starting bit index (0 = LSB)
 * len: number of bits to extract
 *
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_extract_bits_u(uint64_t u64X, unsigned uLo, unsigned uLen)
{
	(void)NULL;
	return b3231_extract(u64X, uLo, uLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_extract_bits_u(uint64_t u64X, unsigned uLo, unsigned uLen)
    __attribute__((alias("gj_u64_extract_bits_u")));
