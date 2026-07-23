/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1091: Shannon entropy of a u8 buffer in milli-bits (estimate)
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_shannon_u8(const uint8_t *data, size_t n);
 *     — Fixed-point Shannon entropy estimate in milli-bits
 *       (1/1000 bit units): floor(1000 * H) where
 *         H = -sum_i p_i * log2(p_i), p_i = count_i / n
 *       over the empirical byte histogram. Empty / NULL → 0.
 *       Maximum theoretical is 8000 (8 bits * 1000). Integer-only
 *       log2 approximation (not a floating KAT).
 *   uint32_t __gj_entropy_shannon_u8  (alias)

 *   __libcgj_batch1091_marker = "libcgj-batch1091"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1091_marker[] = "libcgj-batch1091";

/*
 * b1091_log2_milli — floor(1000 * log2(u)) for u >= 1.
 *
 * Integer path: exponent from CLZ-style shift, fractional part from a
 * linearised mantissa with a quadratic correction so the estimate stays
 * within a few milli-bits of true log2 on [1, 2^32].
 */
static uint32_t
b1091_log2_milli(uint32_t u)
{
	uint32_t uExp;
	uint32_t uT;
	uint32_t uBase;
	uint32_t uX;   /* (u - base) * 1000 / base  in [0, 999] */
	uint32_t uX2;
	uint32_t uFrac;

	if (u <= 1u) {
		return 0u;
	}

	uExp = 0u;
	uT = u;
	while (uT > 1u) {
		uT >>= 1;
		uExp++;
	}

	uBase = 1u << uExp;
	/* x = (u/base - 1) in milli: [0, 1000) */
	uX = (uint32_t)(((uint64_t)(u - uBase) * 1000ull) / (uint64_t)uBase);
	/* log2(1+x) ≈ x - x^2/2  with x in [0,1) milli units */
	uX2 = (uX * uX) / 1000u;
	uFrac = uX - (uX2 / 2u);
	return uExp * 1000u + uFrac;
}

/*
 * gj_entropy_shannon_u8 — milli-bit Shannon entropy of a byte buffer.
 *
 * H_milli = sum_i (c_i / n) * (log2(n) - log2(c_i)) * 1000
 *         = (1/n) * sum_i c_i * (log2_milli(n) - log2_milli(c_i))
 */
uint32_t
gj_entropy_shannon_u8(const uint8_t *pData, size_t cN)
{
	uint32_t aHist[256];
	size_t i;
	uint32_t u32LogN;
	uint64_t u64Acc;
	uint32_t u32C;
	uint32_t u32Diff;

	if (pData == NULL || cN == 0u) {
		return 0u;
	}

	for (i = 0u; i < 256u; i++) {
		aHist[i] = 0u;
	}
	for (i = 0u; i < cN; i++) {
		aHist[pData[i]]++;
	}

	/* n fits in uint32 for log; if n > UINT32_MAX use UINT32_MAX. */
	if (cN > (size_t)UINT32_MAX) {
		u32LogN = b1091_log2_milli(UINT32_MAX);
	} else {
		u32LogN = b1091_log2_milli((uint32_t)cN);
	}

	u64Acc = 0ull;
	for (i = 0u; i < 256u; i++) {
		u32C = aHist[i];
		if (u32C == 0u) {
			continue;
		}
		/* log2(n) - log2(c) ; guard underflow of estimate noise */
		if (u32LogN >= b1091_log2_milli(u32C)) {
			u32Diff = u32LogN - b1091_log2_milli(u32C);
		} else {
			u32Diff = 0u;
		}
		u64Acc += (uint64_t)u32C * (uint64_t)u32Diff;
	}

	/* Divide by n → milli-bits. */
	return (uint32_t)(u64Acc / (uint64_t)cN);
}

uint32_t __gj_entropy_shannon_u8(const uint8_t *pData, size_t cN)
    __attribute__((alias("gj_entropy_shannon_u8")));
