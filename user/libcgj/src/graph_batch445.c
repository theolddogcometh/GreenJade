/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch445: length-n int32 vector add.
 *
 * Surface (unique symbols):
 *   void gj_vec_add_i32(const int32_t *a, const int32_t *b, int32_t *out,
 *                       size_t n);
 *     — out[i] = a[i] + b[i] for i in [0, n). int32 wrap on overflow.
 *       No-op if any pointer is NULL or n is 0. out may equal a or b
 *       (in-place OK). Distinct from matrix helpers (batch226/441).
 *   void __gj_vec_add_i32  (alias)
 *   __libcgj_batch445_marker = "libcgj-batch445"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch445_marker[] = "libcgj-batch445";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vec_add_i32 — element-wise int32 vector sum of length n.
 *
 * a / b: input vectors of length n
 * out:   output vector of length n; may equal a or b
 * n:     element count
 */
void
gj_vec_add_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut, size_t n)
{
	size_t i;

	if (pA == NULL || pB == NULL || pOut == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		pOut[i] = pA[i] + pB[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_vec_add_i32(const int32_t *pA, const int32_t *pB, int32_t *pOut,
                      size_t n)
    __attribute__((alias("gj_vec_add_i32")));
