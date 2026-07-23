/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch954: 3x3 matrix determinant int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_mat3_det_i32(const int32_t m[9]);
 *     — Return det(m) for row-major 3x3. NULL m → 0.
 *   int32_t __gj_mat3_det_i32  (alias)
 *   __libcgj_batch954_marker = "libcgj-batch954"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch954_marker[] = "libcgj-batch954";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat3_det_i32 — determinant of a row-major 3x3 int32 matrix.
 *
 * det = a00(a11*a22 - a12*a21)
 *     - a01(a10*a22 - a12*a20)
 *     + a02(a10*a21 - a11*a20)
 */
int32_t
gj_mat3_det_i32(const int32_t *pM)
{
	int32_t a00, a01, a02;
	int32_t a10, a11, a12;
	int32_t a20, a21, a22;

	if (pM == NULL) {
		return 0;
	}

	a00 = pM[0];
	a01 = pM[1];
	a02 = pM[2];
	a10 = pM[3];
	a11 = pM[4];
	a12 = pM[5];
	a20 = pM[6];
	a21 = pM[7];
	a22 = pM[8];

	return a00 * (a11 * a22 - a12 * a21) -
	       a01 * (a10 * a22 - a12 * a20) +
	       a02 * (a10 * a21 - a11 * a20);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_mat3_det_i32(const int32_t *pM)
    __attribute__((alias("gj_mat3_det_i32")));
