/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2427: int32_t vector dot product with
 * saturating accumulation (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   int64_t gj_i32_dot_n(const int32_t *a, const int32_t *b, size_t n);
 *     - Return sum_i a[i]*b[i] as int64_t. Each product widens to i64
 *       (i32*i32 always fits in i64). Running sum uses portable signed
 *       sat add at INT64_MAX / INT64_MIN. NULL a or NULL b or n == 0
 *       yields 0.
 *   int64_t __gj_i32_dot_n  (alias)
 *   __libcgj_batch2427_marker = "libcgj-batch2427"
 *
 * Array reduce exclusive wave (2421-2430). Distinct from gj_dot_i32
 * (batch539, non-saturating acc) and gj_u32_dot_n (batch2426) - unique
 * i32_dot_n sat surface only; no multi-def. Portable sat mul/add only;
 * no __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2427_marker[] = "libcgj-batch2427";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the int64_t domain. */
static int64_t
b2427_sat_add_i64(int64_t i64A, int64_t i64B)
{
	if (i64B > (int64_t)0) {
		if (i64A > (INT64_MAX - i64B)) {
			return INT64_MAX;
		}
	} else if (i64B < (int64_t)0) {
		if (i64A < (INT64_MIN - i64B)) {
			return INT64_MIN;
		}
	}
	return i64A + i64B;
}

/* Exact i32 * i32 product in i64 (always fits; widen before mul). */
static int64_t
b2427_mul_i32(int32_t i32A, int32_t i32B)
{
	return (int64_t)i32A * (int64_t)i32B;
}

/*
 * Saturating dot of a[0..cN) with b[0..cN). Caller guarantees both
 * pointers non-NULL when cN > 0.
 */
static int64_t
b2427_dot(const int32_t *pA, const int32_t *pB, size_t cN)
{
	size_t i;
	int64_t i64Acc;

	i64Acc = (int64_t)0;
	for (i = 0u; i < cN; i++) {
		i64Acc = b2427_sat_add_i64(i64Acc,
		    b2427_mul_i32(pA[i], pB[i]));
	}
	return i64Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_dot_n - int32 vector inner product with sat i64 accumulate.
 *
 * a, b: length-n vectors (NULL either -> 0)
 * n:    element count (0 -> 0)
 */
int64_t
gj_i32_dot_n(const int32_t *a, const int32_t *b, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL) {
		return (int64_t)0;
	}
	return b2427_dot(a, b, n);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i32_dot_n(const int32_t *a, const int32_t *b, size_t n)
    __attribute__((alias("gj_i32_dot_n")));
