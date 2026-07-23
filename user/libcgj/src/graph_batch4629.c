/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4629: entropy seed-sequence update (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_entropy_seed_seq_u(uint32_t *s, uint32_t x);
 *     - Fold x into *s (seed-sequence style): advance *s with an LCG-
 *       like step mixed with x, store the new seed, return it.
 *       NULL s -> 0 (no write).
 *   uint32_t __gj_entropy_seed_seq_u  (alias)
 *   __libcgj_batch4629_marker = "libcgj-batch4629"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630). Unique
 * gj_entropy_seed_seq_u surface only; no multi-def. Distinct from
 * gj_splitmix64_next (batch714) — 32-bit seed pointer + external x.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4629_marker[] = "libcgj-batch4629";

/* seed_seq-style LCG multiplier / additive mix (odd). */
#define B4629_LCG  0x2c1b3c6du
#define B4629_ADD  0x9e3779b9u
#define B4629_M0   0x85ebca77u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4629_seed_seq(uint32_t *pS, uint32_t u32X)
{
	uint32_t u32S;
	uint32_t u32T;

	u32S = *pS;
	u32T = u32S * B4629_LCG + B4629_ADD + u32X;
	u32T ^= u32T >> 16;
	u32T *= B4629_M0;
	u32T ^= u32T >> 13;
	u32T += u32S;
	*pS = u32T;
	return u32T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_entropy_seed_seq_u - update a 32-bit seed sequence with value x.
 *
 * s: pointer to seed state (updated in place); NULL yields 0
 * x: entropy contribution to fold in
 *
 * Returns the updated seed (also stored at *s when s is non-NULL).
 * No parent wires.
 */
uint32_t
gj_entropy_seed_seq_u(uint32_t *pS, uint32_t u32X)
{
	(void)NULL;
	if (pS == NULL) {
		return 0u;
	}
	return b4629_seed_seq(pS, u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_entropy_seed_seq_u(uint32_t *pS, uint32_t u32X)
    __attribute__((alias("gj_entropy_seed_seq_u")));
