/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1451: 2x2 matrix multiply int32 row-major.
 *
 * Surface (unique symbols):
 *   void gj_mat2_i32_mul(const int32_t a[4], const int32_t b[4],
 *                        int32_t out[4]);
 *     — out = a * b (row-major 2x2). NULL any arg → no-op. out may
 *       alias a/b via a temporary 4-int stack buffer.
 *   void __gj_mat2_i32_mul  (alias)
 *   __libcgj_batch1451_marker = "libcgj-batch1451"
 *
 * Distinct from gj_mat3_mul_i32 (batch952) and gj_matmul_i32 (batch226).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1451_marker[] = "libcgj-batch1451";

/* ---- freestanding helpers ---------------------------------------------- */

/* Copy 4 int32 elements (row-major 2x2). */
static void
b1451_copy4(int32_t *pDst, const int32_t *pSrc)
{
	pDst[0] = pSrc[0];
	pDst[1] = pSrc[1];
	pDst[2] = pSrc[2];
	pDst[3] = pSrc[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_i32_mul — 2x2 matrix product out = a * b (int32 row-major).
 *
 * Layout: [0 1]
 *         [2 3]
 * out00 = a00*b00 + a01*b10
 * out01 = a00*b01 + a01*b11
 * out10 = a10*b00 + a11*b10
 * out11 = a10*b01 + a11*b11
 */
void
gj_mat2_i32_mul(const int32_t *pA, const int32_t *pB, int32_t *pOut)
{
	int32_t i32Tmp[4];
	int32_t i32A00, i32A01, i32A10, i32A11;
	int32_t i32B00, i32B01, i32B10, i32B11;

	if (pA == NULL || pB == NULL || pOut == NULL) {
		return;
	}

	i32A00 = pA[0];
	i32A01 = pA[1];
	i32A10 = pA[2];
	i32A11 = pA[3];
	i32B00 = pB[0];
	i32B01 = pB[1];
	i32B10 = pB[2];
	i32B11 = pB[3];

	i32Tmp[0] = i32A00 * i32B00 + i32A01 * i32B10;
	i32Tmp[1] = i32A00 * i32B01 + i32A01 * i32B11;
	i32Tmp[2] = i32A10 * i32B00 + i32A11 * i32B10;
	i32Tmp[3] = i32A10 * i32B01 + i32A11 * i32B11;

	b1451_copy4(pOut, i32Tmp);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat2_i32_mul(const int32_t *pA, const int32_t *pB, int32_t *pOut)
    __attribute__((alias("gj_mat2_i32_mul")));
