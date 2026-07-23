/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch496: freestanding weighted single-sample index.
 *
 * Surface (unique symbols):
 *   int gj_weighted_sample_one(const uint32_t *w, size_t n, uint32_t r,
 *                              size_t *out);
 *     — Weighted reservoir sample of one index from w[0..n).
 *       w[i] are non-negative item weights; sum = Σ w[i] (uint64_t).
 *       r is a single uniform draw on the full 32-bit range
 *       [0, UINT32_MAX] (i.e. [0, 2^32)). Threshold is the multiply-high
 *       map thr = ((uint64_t)r * sum) >> 32, which lands in [0, sum).
 *       Walks cumulative weights left-to-right and selects the least
 *       index i such that Σ w[0..i] > thr. Zero-weight items are never
 *       chosen when sum > 0. On success *out = i and return is 0.
 *       Returns -1 on error (NULL w/out, n == 0, or all-zero weights);
 *       *out is undefined on error.
 *   int __gj_weighted_sample_one  (alias)
 *   __libcgj_batch496_marker = "libcgj-batch496"
 *
 * Distinct from gj_weighted_choice (batch305), which expects r already
 * in [0, sum). This entry maps a full-range uint32 draw into the weight
 * space (k=1 weighted reservoir / discrete weighted sample).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch496_marker[] = "libcgj-batch496";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_weighted_sample_one — one weighted index from a full-range uint32 draw.
 *
 * w:   weight array (required; NULL rejected)
 * n:   number of weights (0 rejected)
 * r:   uniform integer in [0, UINT32_MAX] (full uint32 domain)
 * out: on success, set to the chosen index in [0, n)
 *
 * Algorithm:
 *   1. sum = Σ w[i] as uint64_t (handles total > UINT32_MAX)
 *   2. thr = (r * sum) >> 32  ∈ [0, sum) when sum > 0
 *   3. walk cumulative; pick first bucket that covers thr
 *
 * Returns 0 on success, -1 on bad input / empty weight mass.
 */
int
gj_weighted_sample_one(const uint32_t *w, size_t n, uint32_t r, size_t *out)
{
	size_t i;
	uint64_t uSum;
	uint64_t uThr;
	uint64_t uAcc;

	if (w == NULL || out == NULL || n == 0u) {
		return -1;
	}

	uSum = 0u;
	for (i = 0u; i < n; i++) {
		uSum += (uint64_t)w[i];
	}
	if (uSum == 0u) {
		return -1;
	}

	/*
	 * Multiply-high: thr in [0, sum). For sum >= 1 and r in [0, 2^32),
	 * floor(r * sum / 2^32) is always strictly less than sum.
	 */
	uThr = ((uint64_t)r * uSum) >> 32;

	uAcc = 0u;
	for (i = 0u; i < n; i++) {
		uAcc += (uint64_t)w[i];
		if (uThr < uAcc) {
			*out = i;
			return 0;
		}
	}

	/* Unreachable when sum > 0 and thr < sum; defensive fail. */
	return -1;
}

int __gj_weighted_sample_one(const uint32_t *w, size_t n, uint32_t r,
    size_t *out)
    __attribute__((alias("gj_weighted_sample_one")));
