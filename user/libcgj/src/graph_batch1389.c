/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1389: extract a bitfield [lo, lo+w) from a u64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_extract_bits(uint64_t x, unsigned lo, unsigned w);
 *     — Extract w consecutive bits starting at bit index lo (0 = LSB).
 *       Result is right-aligned. w == 0 or lo >= 64 → 0. If lo + w
 *       would pass bit 63, only the remaining high bits are taken
 *       (no undefined shift).
 *   uint64_t __gj_u64_extract_bits  (alias)
 *   __libcgj_batch1389_marker = "libcgj-batch1389"
 *
 * Distinct from gj_extract_bits_u64 (batch593) — unique name / surface.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1389_marker[] = "libcgj-batch1389";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Mask of the low uW bits of a u64. uW must be in 1..63; caller handles
 * the full-width (64) and empty (0) cases separately to avoid (1<<64).
 */
static uint64_t
b1389_mask_lo(unsigned uW)
{
	return (UINT64_C(1) << uW) - UINT64_C(1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_extract_bits — right-aligned bitfield extract over [lo, lo+w).
 *
 * u64X: source word
 * uLo:  starting bit index (0 = LSB)
 * uW:   field width in bits
 */
uint64_t
gj_u64_extract_bits(uint64_t u64X, unsigned uLo, unsigned uW)
{
	unsigned uRem;

	if (uW == 0u || uLo >= 64u) {
		return UINT64_C(0);
	}

	uRem = 64u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	u64X >>= uLo;
	if (uW == 64u) {
		/* lo was 0 and full width requested — whole word */
		return u64X;
	}

	return u64X & b1389_mask_lo(uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_extract_bits(uint64_t u64X, unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u64_extract_bits")));
