/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch956: 3-vector cross product int32.
 *
 * Surface (unique symbols):
 *   void gj_vec3_cross_i32(const int32_t a[3], const int32_t b[3],
 *                          int32_t out[3]);
 *     — out = a × b. NULL any → no-op. out may alias a/b via temps.
 *   void __gj_vec3_cross_i32  (alias)
 *   __libcgj_batch956_marker = "libcgj-batch956"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch956_marker[] = "libcgj-batch956";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec3_cross_i32 — integer cross product out = a × b.
 *
 * out0 = a1*b2 - a2*b1
 * out1 = a2*b0 - a0*b2
 * out2 = a0*b1 - a1*b0
 */
void
gj_vec3_cross_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut)
{
	int32_t n0;
	int32_t n1;
	int32_t n2;

	if (pA == NULL || pB == NULL || pOut == NULL) {
		return;
	}

	n0 = pA[1] * pB[2] - pA[2] * pB[1];
	n1 = pA[2] * pB[0] - pA[0] * pB[2];
	n2 = pA[0] * pB[1] - pA[1] * pB[0];
	pOut[0] = n0;
	pOut[1] = n1;
	pOut[2] = n2;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_vec3_cross_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut)
    __attribute__((alias("gj_vec3_cross_i32")));
