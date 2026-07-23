/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch212: in-place Fisher–Yates shuffle of uint32_t[].
 *
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party PRNG/shuffle source copied.
 * No malloc, no errno (freestanding-safe).
 *
 * Surface (unique symbols):
 *   void gj_shuffle_u32(uint32_t *a, size_t n, uint64_t *rng_state);
 *     — Fisher–Yates (modern, Knuth) in-place shuffle of a[0..n).
 *       PRNG is a 64-bit LCG: s <- s * 6364136223846793005 + 1 (mod 2^64),
 *       advanced once per random index. Index selection uses high 32 bits
 *       scaled into [0, i] (Lemire-style multiply-high) to avoid low-bit
 *       LCG bias. *rng_state is updated. No-op if a or rng_state is NULL
 *       or n < 2.
 *   __gj_shuffle_u32  (alias)
 *   __libcgj_batch212_marker = "libcgj-batch212"
 *
 * Distinct from ChaCha/arc4random surfaces (batch106) and rand48 (rand48.c /
 * batch88); this is a deterministic, caller-owned LCG stream for unit tests
 * and graph bring-up, not a CSPRNG.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch212_marker[] = "libcgj-batch212";

/* Numerical Recipes / Knuth MMIX-family 64-bit LCG multiplier. */
#define B212_LCG_A 6364136223846793005ull
#define B212_LCG_C 1ull

/* ---- LCG helpers ------------------------------------------------------- */

/*
 * One LCG step; returns the updated full 64-bit state.
 * Caller keeps *ps as the only entropy/state for the stream.
 */
static uint64_t
b212_lcg_next(uint64_t *ps)
{
	uint64_t s;

	s = *ps * B212_LCG_A + B212_LCG_C;
	*ps = s;
	return s;
}

/*
 * Uniform index in [0, bound) for bound in 1..2^32 (size_t, used as i+1).
 * Uses high 32 bits of the LCG output times bound, then high 32 of product
 * — same shape as Lemire's almost-division-free bounded random for 32-bit
 * ranges (exact when bound is a power of two; otherwise tiny residual bias
 * acceptable for bring-up shuffle).
 */
static size_t
b212_rand_index(uint64_t *ps, size_t bound)
{
	uint32_t r;
	uint64_t prod;

	if (bound <= 1u) {
		return 0u;
	}
	r = (uint32_t)(b212_lcg_next(ps) >> 32);
	prod = (uint64_t)r * (uint64_t)bound;
	return (size_t)(prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shuffle_u32 — Fisher–Yates shuffle of uint32_t array a[0..n).
 *
 * For i = n-1 .. 1:
 *   j = uniform in [0, i]
 *   swap a[i], a[j]
 *
 * rng_state must be non-NULL; caller seeds and owns the stream. The same
 * seed yields a reproducible permutation sequence across runs.
 */
void
gj_shuffle_u32(uint32_t *a, size_t n, uint64_t *rng_state)
{
	size_t i;

	if (a == NULL || rng_state == NULL || n < 2u) {
		return;
	}

	for (i = n - 1u; i > 0u; i--) {
		size_t j;
		uint32_t t;

		j = b212_rand_index(rng_state, i + 1u);
		t = a[i];
		a[i] = a[j];
		a[j] = t;
	}
}

void __gj_shuffle_u32(uint32_t *a, size_t n, uint64_t *rng_state)
    __attribute__((alias("gj_shuffle_u32")));
