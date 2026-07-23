/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch304: reservoir sample of k indices from [0, n).
 *
 * Surface (unique symbols):
 *   int gj_reservoir_u32(size_t n, size_t k, uint32_t *out,
 *                        uint32_t *rng_state);
 *     — Algorithm R (Vitter): sample k distinct indices uniformly from
 *       the population {0, 1, ..., n-1} without replacement. Writes the
 *       chosen indices into out[0..k). Order in out is not sorted.
 *       PRNG is a 32-bit LCG owned by the caller:
 *         s <- s * 1664525 + 1013904223  (mod 2^32)
 *       advanced once per random index. Index selection uses multiply-high
 *       of the full 32-bit LCG output (Lemire-style) to reduce low-bit bias.
 *       *rng_state is updated. Returns 0 on success, -1 on invalid args:
 *         k > n, n > UINT32_MAX, (k > 0 && out == NULL), rng_state == NULL.
 *       k == 0 succeeds (no writes) when rng_state is non-NULL.
 *   __gj_reservoir_u32  (alias)
 *   __libcgj_batch304_marker = "libcgj-batch304"
 *
 * Distinct from Fisher–Yates shuffle of an existing array (batch212); this
 * samples indices only. Deterministic, caller-owned LCG — not a CSPRNG.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch304_marker[] = "libcgj-batch304";

/* Numerical Recipes 32-bit LCG parameters (mod 2^32). */
#define B304_LCG_A 1664525u
#define B304_LCG_C 1013904223u

/* ---- LCG helpers ------------------------------------------------------- */

/*
 * One LCG step; returns the updated full 32-bit state.
 * Caller keeps *pState as the only entropy/state for the stream.
 */
static uint32_t
b304_lcg_next(uint32_t *pState)
{
	uint32_t uS;

	uS = *pState * B304_LCG_A + B304_LCG_C;
	*pState = uS;
	return uS;
}

/*
 * Uniform index in [0, bound) for bound in 1..2^32 (as size_t).
 * Uses full 32-bit LCG output times bound, then high 32 of product —
 * same shape as Lemire's almost-division-free bounded random for 32-bit
 * ranges (exact when bound is a power of two; otherwise tiny residual
 * bias acceptable for bring-up sampling).
 *
 * bound == 0 is treated as 0 (defensive; callers pass i+1 with i >= 0).
 * When bound > UINT32_MAX (only possible as 2^32 on 64-bit size_t), the
 * full uint32 range is already uniform, so return the raw LCG word.
 */
static size_t
b304_rand_index(uint32_t *pState, size_t cBound)
{
	uint32_t uR;
	uint64_t u64Prod;

	if (cBound <= 1u) {
		return 0u;
	}
	uR = b304_lcg_next(pState);
	if (cBound > (size_t)UINT32_MAX) {
		return (size_t)uR;
	}
	u64Prod = (uint64_t)uR * (uint64_t)cBound;
	return (size_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reservoir_u32 — reservoir sample k indices from population size n.
 *
 * Algorithm R (Vitter, without geometric skips — O(n) draws when k < n):
 *   1. out[i] = i for i in 0..k-1  (seed reservoir with first k indices)
 *   2. for t = k .. n-1:
 *        j = uniform in [0, t]   // inclusive upper → bound = t+1
 *        if j < k: out[j] = t
 *
 * Every k-subset of {0..n-1} is equally likely. Indices stored as
 * uint32_t, so n must be at most UINT32_MAX (largest index n-1 fits).
 *
 * rng_state must be non-NULL; caller seeds and owns the stream. The same
 * seed yields a reproducible sample sequence across runs.
 */
int
gj_reservoir_u32(size_t n, size_t k, uint32_t *out, uint32_t *rng_state)
{
	size_t iIdx;
	size_t iT;
	size_t iJ;

	if (rng_state == NULL) {
		return -1;
	}
	if (k == 0u) {
		return 0;
	}
	if (out == NULL || k > n || n > (size_t)UINT32_MAX) {
		return -1;
	}

	/* Seed reservoir with the first k population indices. */
	for (iIdx = 0u; iIdx < k; iIdx++) {
		out[iIdx] = (uint32_t)iIdx;
	}

	/*
	 * For each subsequent index t in [k, n), replace a random
	 * reservoir slot with probability k/(t+1).
	 */
	for (iT = k; iT < n; iT++) {
		iJ = b304_rand_index(rng_state, iT + 1u);
		if (iJ < k) {
			out[iJ] = (uint32_t)iT;
		}
	}
	return 0;
}

int __gj_reservoir_u32(size_t n, size_t k, uint32_t *out, uint32_t *rng_state)
    __attribute__((alias("gj_reservoir_u32")));
