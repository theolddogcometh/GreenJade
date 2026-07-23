/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4237: simple 32-bit Numerical Recipes LCG step.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pcg_step_u(uint32_t *state);
 *     - One 32-bit LCG step (mod 2^32):
 *         *state = *state * 1664525 + 1013904223
 *       Returns the updated *state. NULL state -> 0 (no write).
 *       Named pcg_step historically for the continuum wave; algorithm is
 *       the classic Numerical Recipes LCG (not full PCG-XSH-RR).
 *   uint32_t __gj_u32_pcg_step_u  (alias)
 *   __libcgj_batch4237_marker = "libcgj-batch4237"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_u32_pcg_step_u surface
 * only; no multi-def. Distinct from gj_pcg32_next_u (batch2186 full
 * PCG-XSH-RR). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4237_marker[] = "libcgj-batch4237";

/* Numerical Recipes 32-bit LCG parameters (mod 2^32). */
#define B4237_LCG_A  1664525u
#define B4237_LCG_C  1013904223u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pcg_step_u - one 32-bit LCG step; updates *state.
 *
 * state: caller-owned 32-bit word. NULL -> returns 0 without writing.
 *
 * *state <- *state * 1664525 + 1013904223 (mod 2^32); returns new value.
 * No parent wires.
 */
uint32_t
gj_u32_pcg_step_u(uint32_t *state)
{
	uint32_t u32S;

	(void)NULL;

	if (state == NULL) {
		return 0u;
	}

	u32S = *state * B4237_LCG_A + B4237_LCG_C;
	*state = u32S;
	return u32S;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pcg_step_u(uint32_t *state)
    __attribute__((alias("gj_u32_pcg_step_u")));
