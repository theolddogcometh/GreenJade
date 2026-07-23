/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch955: 3-vector dot product int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_vec3_dot_i32(const int32_t a[3], const int32_t b[3]);
 *     — Return a0*b0 + a1*b1 + a2*b2. NULL either → 0.
 *   int32_t __gj_vec3_dot_i32  (alias)
 *   __libcgj_batch955_marker = "libcgj-batch955"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch955_marker[] = "libcgj-batch955";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec3_dot_i32 — integer dot product of two 3-vectors.
 */
int32_t
gj_vec3_dot_i32(const int32_t *pA, const int32_t *pB)
{
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	return pA[0] * pB[0] + pA[1] * pB[1] + pA[2] * pB[2];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_vec3_dot_i32(const int32_t *pA, const int32_t *pB)
    __attribute__((alias("gj_vec3_dot_i32")));
