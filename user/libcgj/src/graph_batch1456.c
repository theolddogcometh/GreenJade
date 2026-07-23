/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1456: 2D cross-product z-magnitude int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_vec2_i32_cross_z(const int32_t a[2], const int32_t b[2]);
 *     — Return the scalar z-component of a × b in 2D:
 *         a0*b1 - a1*b0
 *       (signed parallelogram area / oriented magnitude). NULL either → 0.
 *   int32_t __gj_vec2_i32_cross_z  (alias)
 *   __libcgj_batch1456_marker = "libcgj-batch1456"
 *
 * Distinct from gj_vec3_cross_i32 (batch956). Unique surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1456_marker[] = "libcgj-batch1456";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec2_i32_cross_z — 2D cross product magnitude (z component).
 *
 * For a=(ax,ay), b=(bx,by): ax*by - ay*bx.
 * Positive when b is left of a (CCW from a to b).
 */
int32_t
gj_vec2_i32_cross_z(const int32_t *pA, const int32_t *pB)
{
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	return pA[0] * pB[1] - pA[1] * pB[0];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_vec2_i32_cross_z(const int32_t *pA, const int32_t *pB)
    __attribute__((alias("gj_vec2_i32_cross_z")));
