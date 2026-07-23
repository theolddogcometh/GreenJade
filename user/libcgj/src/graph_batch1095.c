/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1095: fixed-point milli-cosine of two u8 vectors
 *
 * Surface (unique symbols):
 *   int32_t gj_cosine_u8(const uint8_t *a, const uint8_t *b, size_t n);
 *     — Milli-cosine in [0, 1000]:
 *         floor((dot * 1000) / (l2(a) * l2(b)))
 *       with l2(v) = floor(isqrt(sum v[i]^2)). Zero either vector or
 *       NULL a/b or n == 0 → 0. Distinct from gj_cosine_i32.
 *   int32_t __gj_cosine_u8  (alias)
 *   __libcgj_batch1095_marker = "libcgj-batch1095"
 *
 * Distinct from gj_cosine_i32 (batch556) — avoid multi-def.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1095_marker[] = "libcgj-batch1095";

static uint64_t
b1095_isqrt(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1u) {
		return u64N;
	}

	u64Lo = 1u;
	u64Hi = (u64N < 0xffffffffULL) ? u64N : 0xffffffffULL;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1u) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1u;
		}
	}
	return u64Lo;
}

/*
 * gj_cosine_u8 — milli-cosine of two length-n uint8 vectors.
 */
int32_t
gj_cosine_u8(const uint8_t *pA, const uint8_t *pB, size_t cN)
{
	uint64_t u64Dot;
	uint64_t u64Sa;
	uint64_t u64Sb;
	uint64_t u64La;
	uint64_t u64Lb;
	uint64_t u64Denom;
	uint64_t u64Res;
	size_t i;

	if (pA == NULL || pB == NULL || cN == 0u) {
		return 0;
	}

	u64Dot = 0ull;
	u64Sa = 0ull;
	u64Sb = 0ull;

	for (i = 0u; i < cN; i++) {
		uint64_t uA = (uint64_t)pA[i];
		uint64_t uB = (uint64_t)pB[i];

		u64Dot += uA * uB;
		u64Sa += uA * uA;
		u64Sb += uB * uB;
	}

	u64La = b1095_isqrt(u64Sa);
	u64Lb = b1095_isqrt(u64Sb);
	if (u64La == 0ull || u64Lb == 0ull) {
		return 0;
	}

	u64Denom = u64La * u64Lb;
	if (u64Denom == 0ull) {
		return 0;
	}

	u64Res = (u64Dot * 1000ull) / u64Denom;
	if (u64Res > 1000ull) {
		u64Res = 1000ull;
	}
	return (int32_t)u64Res;
}

int32_t __gj_cosine_u8(const uint8_t *pA, const uint8_t *pB, size_t cN)
    __attribute__((alias("gj_cosine_u8")));
