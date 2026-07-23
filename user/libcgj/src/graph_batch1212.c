/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1212: elementwise absolute value into out.
 *
 * Surface (unique symbols):
 *   void gj_i32_abs_arr(const int32_t *in, int32_t *out, size_t n);
 *     — out[i] = |in[i]|. INT32_MIN maps to INT32_MAX (saturate; no UB).
 *       NULL in or out with n > 0 → no-op. n == 0 → no-op.
 *       in and out may alias.
 *   void __gj_i32_abs_arr  (alias)
 *   __libcgj_batch1212_marker = "libcgj-batch1212"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1212_marker[] = "libcgj-batch1212";

#define B1212_I32_MIN  ((int32_t)0x80000000)
#define B1212_I32_MAX  ((int32_t)0x7fffffff)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_abs_arr — absolute value of each element into out.
 */
void
gj_i32_abs_arr(const int32_t *pIn, int32_t *pOut, size_t cN)
{
	size_t i;

	if (pIn == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		int32_t i32V;

		i32V = pIn[i];
		if (i32V < 0) {
			if (i32V == B1212_I32_MIN) {
				i32V = B1212_I32_MAX;
			} else {
				i32V = -i32V;
			}
		}
		pOut[i] = i32V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_abs_arr(const int32_t *pIn, int32_t *pOut, size_t cN)
    __attribute__((alias("gj_i32_abs_arr")));
