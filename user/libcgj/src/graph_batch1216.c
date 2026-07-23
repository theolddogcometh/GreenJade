/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1216: elementwise int32 multiply into out.
 *
 * Surface (unique symbols):
 *   void gj_i32_mul_arr(const int32_t *a, const int32_t *b, int32_t *out,
 *                       size_t n);
 *     — out[i] = a[i] * b[i] with modular wrap (unsigned intermediate).
 *       Any NULL a/b/out with n > 0 → no-op. n == 0 → no-op.
 *   void __gj_i32_mul_arr  (alias)
 *   __libcgj_batch1216_marker = "libcgj-batch1216"
 *
 * Unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1216_marker[] = "libcgj-batch1216";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_mul_arr — elementwise modular multiplication into out.
 */
void
gj_i32_mul_arr(const int32_t *pA, const int32_t *pB, int32_t *pOut,
    size_t cN)
{
	size_t i;

	if (pA == NULL || pB == NULL || pOut == NULL || cN == 0u) {
		return;
	}

	for (i = 0u; i < cN; i++) {
		uint32_t u32Prod;

		u32Prod = (uint32_t)pA[i] * (uint32_t)pB[i];
		pOut[i] = (int32_t)u32Prod;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_mul_arr(const int32_t *pA, const int32_t *pB, int32_t *pOut,
    size_t cN)
    __attribute__((alias("gj_i32_mul_arr")));
