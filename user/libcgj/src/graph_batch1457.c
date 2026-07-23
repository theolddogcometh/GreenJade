/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1457: 2-vector add int32.
 *
 * Surface (unique symbols):
 *   void gj_vec2_i32_add(const int32_t a[2], const int32_t b[2],
 *                        int32_t out[2]);
 *     — out = a + b element-wise. NULL any → no-op. out may alias a/b.
 *   void __gj_vec2_i32_add  (alias)
 *   __libcgj_batch1457_marker = "libcgj-batch1457"
 *
 * Distinct from gj_vec_add_i32 (batch445). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1457_marker[] = "libcgj-batch1457";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec2_i32_add — element-wise sum of two 2-vectors into out.
 *
 * Uses temps so out may overlap a or b.
 */
void
gj_vec2_i32_add(const int32_t *pA, const int32_t *pB, int32_t *pOut)
{
	int32_t i32X;
	int32_t i32Y;

	if (pA == NULL || pB == NULL || pOut == NULL) {
		return;
	}

	i32X = pA[0] + pB[0];
	i32Y = pA[1] + pB[1];
	pOut[0] = i32X;
	pOut[1] = i32Y;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_vec2_i32_add(const int32_t *pA, const int32_t *pB, int32_t *pOut)
    __attribute__((alias("gj_vec2_i32_add")));
