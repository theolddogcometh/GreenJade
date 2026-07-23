/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2186: reentrant PCG32 next (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcg32_next_u(uint64_t *state, uint64_t *inc);
 *     - Advance *state with PCG-XSH-RR and return one 32-bit sample.
 *       inc is OR'd with 1 each step (odd stream requirement).
 *       NULL state or inc -> 0 (no write on NULL).
 *   uint32_t __gj_pcg32_next_u  (alias)
 *   __libcgj_batch2186_marker = "libcgj-batch2186"
 *
 * Distinct from gj_pcg32_next / gj_pcg32_seed (batch303). Unique _u
 * export for the post-2180 random wave; no multi-def.
 *
 * Algorithm (PCG-XSH-RR / pcg32):
 *   multiplier  = 6364136223846793005
 *   state'      = state * multiplier + (inc | 1)
 *   xorshifted  = ((old >> 18) ^ old) >> 27
 *   rot         = old >> 59
 *   output      = ror32(xorshifted, rot)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2186_marker[] = "libcgj-batch2186";

/* PCG32 LCG multiplier (public PCG family constant). */
#define B2186_PCG_MULT  6364136223846793005ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* 32-bit rotate right by uRot (mod 32). */
static uint32_t
b2186_ror32(uint32_t u32X, uint32_t uRot)
{
	return (u32X >> uRot) | (u32X << ((-uRot) & 31u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcg32_next_u - one 32-bit PCG-XSH-RR sample; updates *state.
 *
 * state / inc: caller-owned 64-bit words. inc is OR'd with 1 each step.
 * Returns 0 if either pointer is NULL.
 */
uint32_t
gj_pcg32_next_u(uint64_t *pState, uint64_t *pInc)
{
	uint64_t u64Old;
	uint32_t u32Xorshifted;
	uint32_t u32Rot;

	(void)NULL;

	if (pState == NULL || pInc == NULL) {
		return 0u;
	}

	u64Old = *pState;
	*pState = u64Old * B2186_PCG_MULT + (*pInc | 1ULL);

	u32Xorshifted = (uint32_t)(((u64Old >> 18u) ^ u64Old) >> 27u);
	u32Rot = (uint32_t)(u64Old >> 59u);
	return b2186_ror32(u32Xorshifted, u32Rot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcg32_next_u(uint64_t *pState, uint64_t *pInc)
    __attribute__((alias("gj_pcg32_next_u")));
