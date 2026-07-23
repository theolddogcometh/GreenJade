/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1452: 2x2 matrix determinant int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_mat2_i32_det(const int32_t m[4]);
 *     — Return det(m) for row-major 2x2: m00*m11 - m01*m10.
 *       NULL m → 0.
 *   int32_t __gj_mat2_i32_det  (alias)
 *   __libcgj_batch1452_marker = "libcgj-batch1452"
 *
 * Distinct from gj_mat3_det_i32 (batch954) and gj_mat_det_i64 (batch253).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1452_marker[] = "libcgj-batch1452";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_i32_det — determinant of a row-major 2x2 int32 matrix.
 *
 * Layout: [0 1]   det = m[0]*m[3] - m[1]*m[2]
 *         [2 3]
 */
int32_t
gj_mat2_i32_det(const int32_t *pM)
{
	if (pM == NULL) {
		return 0;
	}
	return pM[0] * pM[3] - pM[1] * pM[2];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_mat2_i32_det(const int32_t *pM)
    __attribute__((alias("gj_mat2_i32_det")));
