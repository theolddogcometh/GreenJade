/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch952: 3x3 matrix multiply int32 row-major.
 *
 * Surface (unique symbols):
 *   void gj_mat3_mul_i32(const int32_t a[9], const int32_t b[9],
 *                        int32_t out[9]);
 *     — out = a * b (row-major). NULL any arg → no-op. out may alias a/b
 *       via a temporary 9-int stack buffer.
 *   void __gj_mat3_mul_i32  (alias)
 *   __libcgj_batch952_marker = "libcgj-batch952"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch952_marker[] = "libcgj-batch952";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat3_mul_i32 — 3x3 matrix product out = a * b (int32 row-major).
 *
 * Row i of out is row i of a dotted with columns of b.
 * Uses a local temporary so out may overlap a or b.
 */
void
gj_mat3_mul_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut)
{
	int32_t nTmp[9];
	int i;
	int j;
	int k;

	if (pA == NULL || pB == NULL || pOut == NULL) {
		return;
	}

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			int32_t nSum = 0;

			for (k = 0; k < 3; k++) {
				nSum = nSum + pA[i * 3 + k] * pB[k * 3 + j];
			}
			nTmp[i * 3 + j] = nSum;
		}
	}
	for (i = 0; i < 9; i++) {
		pOut[i] = nTmp[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat3_mul_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut)
    __attribute__((alias("gj_mat3_mul_i32")));
