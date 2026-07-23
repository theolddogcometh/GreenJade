/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch556: fixed-point milli-cosine of two int32
 * vectors (integer L2 norms via isqrt of sum-of-squares).
 *
 * Surface (unique symbols):
 *   int32_t gj_cosine_i32(const int32_t *a, const int32_t *b, size_t n);
 *     — milli-cosine in [-1000, 1000]:
 *         (dot * 1000) / (l2(a) * l2(b))
 *       where l2(v) = floor(isqrt(sum_i v[i]^2)). Zero either vector
 *       (or NULL a/b) returns 0. Result is clamped to [-1000, 1000].
 *   __gj_cosine_i32  (alias)
 *   __libcgj_batch556_marker = "libcgj-batch556"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * floating point, no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch556_marker[] = "libcgj-batch556";

/* ---- static integer square root ---------------------------------------- */

/*
 * b556_isqrt — floor integer square root of u64N.
 *
 * Binary search over r in [0, min(n, 2^32-1)]. floor(sqrt(UINT64_MAX))
 * is 4294967295, so the candidate root always fits in 32 bits and
 * mid*mid never overflows uint64_t under that cap.
 */
static uint64_t
b556_isqrt(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1u) {
		return u64N;
	}

	u64Lo = 1u;
	/* Cap high end: largest r with r*r representable in 64 bits. */
	u64Hi = (u64N < 0xffffffffULL) ? u64N : 0xffffffffULL;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1u) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1u;
		}
	}
	return u64Lo;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cosine_i32 — fixed-point milli-cosine of two length-n int32 vectors.
 *
 * a, b: length-n int32 arrays (NULL either → 0)
 * n:    element count (0 → 0; both treated as zero vectors)
 *
 * Computes:
 *   dot  = sum_i (int64)a[i]*(int64)b[i]
 *   sa   = sum_i (int64)a[i]*(int64)a[i]   (as uint64 magnitude)
 *   sb   = sum_i (int64)b[i]*(int64)b[i]
 *   la   = isqrt(sa), lb = isqrt(sb)
 *   if la==0 or lb==0 → 0
 *   else clamp((dot * 1000) / (la * lb), -1000, 1000)
 *
 * Products widen to int64 before multiply so int32 extremes do not wrap
 * the intermediate term. Accumulators are int64/uint64; huge n may wrap.
 */
int32_t
gj_cosine_i32(const int32_t *a, const int32_t *b, size_t n)
{
	int64_t i64Dot;
	uint64_t u64Sa;
	uint64_t u64Sb;
	uint64_t u64La;
	uint64_t u64Lb;
	int64_t i64Denom;
	int64_t i64Num;
	int64_t i64Res;
	size_t i;

	if (a == NULL || b == NULL || n == 0u) {
		return 0;
	}

	i64Dot = (int64_t)0;
	u64Sa = 0ull;
	u64Sb = 0ull;

	for (i = 0u; i < n; i++) {
		int64_t ai;
		int64_t bi;

		ai = (int64_t)a[i];
		bi = (int64_t)b[i];
		i64Dot += ai * bi;
		/* Squares are non-negative; accumulate as unsigned. */
		u64Sa += (uint64_t)(ai * ai);
		u64Sb += (uint64_t)(bi * bi);
	}

	u64La = b556_isqrt(u64Sa);
	u64Lb = b556_isqrt(u64Sb);
	if (u64La == 0ull || u64Lb == 0ull) {
		return 0;
	}

	i64Denom = (int64_t)u64La * (int64_t)u64Lb;
	if (i64Denom == (int64_t)0) {
		return 0;
	}

	i64Num = i64Dot * (int64_t)1000;
	i64Res = i64Num / i64Denom;

	if (i64Res > (int64_t)1000) {
		i64Res = (int64_t)1000;
	} else if (i64Res < (int64_t)-1000) {
		i64Res = (int64_t)-1000;
	}
	return (int32_t)i64Res;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cosine_i32(const int32_t *a, const int32_t *b, size_t n)
    __attribute__((alias("gj_cosine_i32")));
