/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3724: freestanding PCG32 (XSH-RR) next (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rng_pcg32_u(uint64_t *state, uint64_t *inc);
 *     - Advance *state with the PCG LCG and return one 32-bit XSH-RR
 *       sample. inc is OR'd with 1 each step so the stream id stays
 *       odd. NULL state or inc -> 0 (no write).
 *   uint32_t __gj_rng_pcg32_u  (alias)
 *   __libcgj_batch3724_marker = "libcgj-batch3724"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_pcg32_next / gj_pcg32_seed (batch303) — unique rng_*_u next-only
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Algorithm (PCG-XSH-RR / pcg32, public family constants):
 *   multiplier  = 6364136223846793005
 *   state'      = state * multiplier + (inc | 1)
 *   xorshifted  = ((old >> 18) ^ old) >> 27
 *   rot         = old >> 59
 *   output      = ror32(xorshifted, rot)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Not cryptographically secure. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3724_marker[] = "libcgj-batch3724";

/* PCG32 LCG multiplier (public PCG family constant). */
#define B3724_PCG_MULT  6364136223846793005ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* 32-bit rotate right by uRot (0..31). */
static uint32_t
b3724_ror32(uint32_t u32X, uint32_t uRot)
{
	return (u32X >> uRot) | (u32X << ((-uRot) & 31u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_pcg32_u — one 32-bit PCG-XSH-RR sample; updates *state in place.
 *
 * state / inc: caller-owned 64-bit words. inc is OR'd with 1 each step so
 *              a non-odd stream still advances with full period.
 * Returns 0 if either pointer is NULL.
 */
uint32_t
gj_rng_pcg32_u(uint64_t *state, uint64_t *inc)
{
	uint64_t uOld;
	uint32_t uXorshifted;
	uint32_t uRot;

	(void)NULL;

	if (state == NULL || inc == NULL) {
		return 0u;
	}

	uOld = *state;
	/*
	 * Advance with odd increment (PCG requires odd stream id so the
	 * LCG period is full 2^64).
	 */
	*state = uOld * B3724_PCG_MULT + (*inc | 1ULL);

	/* Output function: xorshift high, then random rotate (XSH-RR). */
	uXorshifted = (uint32_t)(((uOld >> 18u) ^ uOld) >> 27u);
	uRot = (uint32_t)(uOld >> 59u);
	return b3724_ror32(uXorshifted, uRot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rng_pcg32_u(uint64_t *state, uint64_t *inc)
    __attribute__((alias("gj_rng_pcg32_u")));
