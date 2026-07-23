/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch539: freestanding int32 vector dot product.
 *
 * Surface (unique symbols):
 *   int64_t gj_dot_i32(const int32_t *a, const int32_t *b, size_t n);
 *     — Return sum_{i=0..n-1} (int64_t)a[i] * (int64_t)b[i].
 *       NULL a or NULL b returns 0. Empty n returns 0.
 *   __gj_dot_i32  (alias)
 *   __libcgj_batch539_marker = "libcgj-batch539"
 *
 * Products are widened to int64 before multiply so int32 extremes do not
 * overflow the intermediate. Accumulator is int64; caller owns any
 * int64 sum overflow on huge n or extreme values.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch539_marker[] = "libcgj-batch539";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dot_i32 — int32 vector inner product accumulated in int64.
 *
 * a, b: length-n vectors (NULL either → 0)
 * n:    element count (0 → 0)
 *
 * Each term is (int64_t)a[i] * (int64_t)b[i] so the multiply is full-width
 * relative to int32 operands. Returns the running sum.
 */
int64_t
gj_dot_i32(const int32_t *a, const int32_t *b, size_t n)
{
	int64_t i64Acc;
	size_t i;

	if (a == NULL || b == NULL) {
		return (int64_t)0;
	}

	i64Acc = (int64_t)0;
	for (i = 0u; i < n; i++) {
		i64Acc += (int64_t)a[i] * (int64_t)b[i];
	}
	return i64Acc;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_dot_i32(const int32_t *a, const int32_t *b, size_t n)
    __attribute__((alias("gj_dot_i32")));
