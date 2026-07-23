/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch716: simple single-state PCG32 (PCG-XSH-RR).
 *
 * Surface (unique symbols for this TU's API shape):
 *   uint32_t gj_pcg32_next_s(uint64_t *state);
 *     — Advance *state with the fixed default PCG stream and return one
 *       32-bit XSH-RR sample. NULL state → return 0 (no errno).
 *   uint32_t __gj_pcg32_next_s_s  (alias)
 *   __libcgj_batch716_marker = "libcgj-batch716"
 *
 * Single-pointer surface (fixed default increment). Distinct arity from
 * any two-pointer (state, inc) reentrant helpers elsewhere; this TU is
 * the simple state-only step.
 *
 * Algorithm (PCG-XSH-RR / pcg32, fixed default stream):
 *   multiplier  = 6364136223846793005
 *   increment   = 1442695040888963407  (odd default stream)
 *   state'      = state * multiplier + increment
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

const char __libcgj_batch716_marker[] = "libcgj-batch716";

/* PCG32 LCG multiplier and default (odd) stream increment. */
#define B716_PCG_MULT  6364136223846793005ULL
#define B716_PCG_INC   1442695040888963407ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcg32_next_s — one 32-bit PCG-XSH-RR sample; updates *state in place.
 *
 * state: caller-owned 64-bit word (NULL → return 0).
 * Uses the fixed default PCG stream increment (always odd).
 */
uint32_t
gj_pcg32_next_s(uint64_t *state)
{
	uint64_t uOld;
	uint32_t uXorshifted;
	uint32_t uRot;

	if (state == NULL) {
		return 0u;
	}

	uOld = *state;
	/* Advance with fixed odd default stream. */
	*state = uOld * B716_PCG_MULT + B716_PCG_INC;

	/* Output function: xorshift high, then random rotate (XSH-RR). */
	uXorshifted = (uint32_t)(((uOld >> 18u) ^ uOld) >> 27u);
	uRot = (uint32_t)(uOld >> 59u);
	return (uXorshifted >> uRot) | (uXorshifted << ((-uRot) & 31u));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcg32_next_s_s(uint64_t *state)
    __attribute__((alias("gj_pcg32_next_s")));
