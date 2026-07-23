/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1454: 2x2 matrix transpose int32 row-major.
 *
 * Surface (unique symbols):
 *   void gj_mat2_i32_transpose(const int32_t m[4], int32_t out[4]);
 *     — out = m^T. NULL any → no-op. out may alias m via temps.
 *   void __gj_mat2_i32_transpose  (alias)
 *   __libcgj_batch1454_marker = "libcgj-batch1454"
 *
 * Distinct from gj_mat_transpose_i32 (batch443). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1454_marker[] = "libcgj-batch1454";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_i32_transpose — 2x2 transpose out = m^T (int32 row-major).
 *
 * Layout in:  [0 1]     out: [0 2]
 *             [2 3]          [1 3]
 * Uses locals so out may overlap m.
 */
void
gj_mat2_i32_transpose(const int32_t *pM, int32_t *pOut)
{
	int32_t i32M00, i32M01, i32M10, i32M11;

	if (pM == NULL || pOut == NULL) {
		return;
	}

	i32M00 = pM[0];
	i32M01 = pM[1];
	i32M10 = pM[2];
	i32M11 = pM[3];

	pOut[0] = i32M00;
	pOut[1] = i32M10;
	pOut[2] = i32M01;
	pOut[3] = i32M11;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_mat2_i32_transpose(const int32_t *pM, int32_t *pOut)
    __attribute__((alias("gj_mat2_i32_transpose")));
