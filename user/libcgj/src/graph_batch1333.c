/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1333: Shannon entropy of a u8 buffer in milli-bits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u8_entropy_milli(const uint8_t *data, size_t n);
 *     — Fixed-point Shannon entropy floor(1000 * H) where
 *       H = -sum p_i log2(p_i) over the empirical byte histogram.
 *       Empty / NULL → 0. Theoretical max ≈ 8000 milli-bits.
 *   uint32_t __gj_u8_entropy_milli  (alias)
 *
 * Distinct from gj_entropy_shannon_u8 (batch1091) — unique name.
 *   __libcgj_batch1333_marker = "libcgj-batch1333"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1333_marker[] = "libcgj-batch1333";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1333_log2_milli — floor(1000 * log2(u)) for u >= 1.
 * Integer exponent + quadratic mantissa correction.
 */
static uint32_t
b1333_log2_milli(uint32_t u)
{
	uint32_t uExp;
	uint32_t uT;
	uint32_t uBase;
	uint32_t uX;
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
	uX = (uint32_t)(((uint64_t)(u - uBase) * 1000ull) / (uint64_t)uBase);
	uX2 = (uX * uX) / 1000u;
	uFrac = uX - (uX2 / 2u);
	return uExp * 1000u + uFrac;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_entropy_milli — milli-bit Shannon entropy of a byte buffer.
 *
 * H_milli = (1/n) * sum_i c_i * (log2_milli(n) - log2_milli(c_i))
 */
uint32_t
gj_u8_entropy_milli(const uint8_t *pData, size_t cN)
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

	if (cN > (size_t)UINT32_MAX) {
		u32LogN = b1333_log2_milli(UINT32_MAX);
	} else {
		u32LogN = b1333_log2_milli((uint32_t)cN);
	}

	u64Acc = 0ull;
	for (i = 0u; i < 256u; i++) {
		u32C = aHist[i];
		if (u32C == 0u) {
			continue;
		}
		if (u32LogN >= b1333_log2_milli(u32C)) {
			u32Diff = u32LogN - b1333_log2_milli(u32C);
		} else {
			u32Diff = 0u;
		}
		u64Acc += (uint64_t)u32C * (uint64_t)u32Diff;
	}

	return (uint32_t)(u64Acc / (uint64_t)cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u8_entropy_milli(const uint8_t *pData, size_t cN)
    __attribute__((alias("gj_u8_entropy_milli")));
