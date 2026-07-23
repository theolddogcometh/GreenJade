/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1213: elementwise negation into out.
 *
 * Surface (unique symbols):
 *   void gj_i32_neg_arr(const int32_t *in, int32_t *out, size_t n);
 *     — out[i] = -in[i]. INT32_MIN stays INT32_MIN (two's-complement
 *       self-inverse; avoids signed-overflow UB). NULL in or out with
 *       n > 0 → no-op. n == 0 → no-op. in and out may alias.
 *   void __gj_i32_neg_arr  (alias)
 *   __libcgj_batch1213_marker = "libcgj-batch1213"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1213_marker[] = "libcgj-batch1213";

#define B1213_I32_MIN  ((int32_t)0x80000000)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_neg_arr — negate each element into out.
 */
void
gj_i32_neg_arr(const int32_t *pIn, int32_t *pOut, size_t cN)
{
	size_t i;

	if (pIn == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		int32_t i32V;

		i32V = pIn[i];
		if (i32V == B1213_I32_MIN) {
			pOut[i] = i32V;
		} else {
			pOut[i] = -i32V;
		}
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_neg_arr(const int32_t *pIn, int32_t *pOut, size_t cN)
    __attribute__((alias("gj_i32_neg_arr")));
