/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch303: PCG32 (PCG-XSH-RR) reentrant PRNG.
 *
 * Surface (unique symbols — distinct from LCG rand/xorshift/arc4random):
 *   void     gj_pcg32_seed(uint64_t *state, uint64_t *inc, uint64_t seed);
 *     — Initialize a caller-owned (state, inc) pair from a single seed.
 *       inc is forced odd (PCG stream requirement). Uses the standard
 *       two-step warm-up (advance, add seed, advance).
 *   uint32_t gj_pcg32_next(uint64_t *state, uint64_t *inc);
 *     — Advance state and return one 32-bit sample (XSH-RR output).
 *       NULL state or inc → return 0 (no errno in freestanding).
 *   __gj_pcg32_seed / __gj_pcg32_next  (aliases)
 *   __libcgj_batch303_marker = "libcgj-batch303"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   rand / srand / random / srandom          → stdlib / rand_div.c
 *   drand48 / lrand48 / srand48 / seed48     → rand48.c
 *   arc4random* / __arc4random*              → graph_batch106.c
 *   gj_xorshift128_*                         → graph_batch173.c
 *   batch35 / batch212 LCG helpers           → fixed-stream LCGs only
 *
 * Algorithm (PCG-XSH-RR / pcg32):
 *   multiplier  = 6364136223846793005
 *   state'      = state * multiplier + (inc | 1)
 *   xorshifted  = ((old >> 18) ^ old) >> 27
 *   rot         = old >> 59
 *   output      = ror32(xorshifted, rot)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Not cryptographically secure. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch303_marker[] = "libcgj-batch303";

/* PCG32 LCG multiplier (public PCG family constant). */
#define B303_PCG_MULT  6364136223846793005ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcg32_next — one 32-bit PCG-XSH-RR sample; updates *state in place.
 *
 * state / inc: caller-owned 64-bit words. inc is OR'd with 1 each step so
 *              a non-odd stream still advances correctly.
 * Returns 0 if either pointer is NULL.
 */
uint32_t
gj_pcg32_next(uint64_t *state, uint64_t *inc)
{
	uint64_t uOld;
	uint32_t uXorshifted;
	uint32_t uRot;

	if (state == NULL || inc == NULL) {
		return 0u;
	}

	uOld = *state;
	/*
	 * Advance with odd increment (PCG requires odd stream id so the
	 * LCG period is full 2^64).
	 */
	*state = uOld * B303_PCG_MULT + (*inc | 1ULL);

	/* Output function: xorshift high, then random rotate (XSH-RR). */
	uXorshifted = (uint32_t)(((uOld >> 18u) ^ uOld) >> 27u);
	uRot = (uint32_t)(uOld >> 59u);
	return (uXorshifted >> uRot) | (uXorshifted << ((-uRot) & 31u));
}

uint32_t __gj_pcg32_next(uint64_t *state, uint64_t *inc)
    __attribute__((alias("gj_pcg32_next")));

/*
 * gj_pcg32_seed — initialize (state, inc) from a single 64-bit seed.
 *
 * Matches the classic pcg32_srandom_r shape with one seed used both as
 * initstate and as the stream selector:
 *   state = 0
 *   inc   = (seed << 1) | 1     // always odd stream
 *   next()
 *   state += seed
 *   next()
 *
 * NULL state or inc is a no-op.
 */
void
gj_pcg32_seed(uint64_t *state, uint64_t *inc, uint64_t seed)
{
	if (state == NULL || inc == NULL) {
		return;
	}

	*state = 0ULL;
	*inc = (seed << 1u) | 1ULL;
	(void)gj_pcg32_next(state, inc);
	*state += seed;
	(void)gj_pcg32_next(state, inc);
}

void __gj_pcg32_seed(uint64_t *state, uint64_t *inc, uint64_t seed)
    __attribute__((alias("gj_pcg32_seed")));
