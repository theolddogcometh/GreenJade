/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1455: 2-vector dot product int32.
 *
 * Surface (unique symbols):
 *   int32_t gj_vec2_i32_dot(const int32_t a[2], const int32_t b[2]);
 *     — Return a0*b0 + a1*b1. NULL either → 0.
 *   int32_t __gj_vec2_i32_dot  (alias)
 *   __libcgj_batch1455_marker = "libcgj-batch1455"
 *
 * Distinct from gj_vec3_dot_i32 (batch955) and gj_dot_i32 (batch539).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1455_marker[] = "libcgj-batch1455";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec2_i32_dot — integer dot product of two 2-vectors.
 */
int32_t
gj_vec2_i32_dot(const int32_t *pA, const int32_t *pB)
{
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	return pA[0] * pB[0] + pA[1] * pB[1];
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_vec2_i32_dot(const int32_t *pA, const int32_t *pB)
    __attribute__((alias("gj_vec2_i32_dot")));
