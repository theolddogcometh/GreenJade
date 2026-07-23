/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3723: freestanding SplitMix64 PRNG step (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rng_splitmix64_u(uint64_t *state);
 *     - One SplitMix64 step: advance *state by the golden-ratio gamma,
 *       then apply the two multiply-xor finalizers. Returns the mixed
 *       64-bit output. state == NULL -> 0 (no write).
 *   uint64_t __gj_rng_splitmix64_u  (alias)
 *   __libcgj_batch3723_marker = "libcgj-batch3723"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_splitmix64 (batch424 pure one-shot) and gj_splitmix64_next
 * (batch714) — unique rng_*_u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Constants match the public SplitMix64 algorithm (Steele /
 * Vigna lineage). No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3723_marker[] = "libcgj-batch3723";

/* SplitMix64 public algorithm constants. */
#define B3723_GAMMA  0x9e3779b97f4a7c15ULL
#define B3723_M1     0xbf58476d1ce4e5b9ULL
#define B3723_M2     0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One SplitMix64 mix of an already-advanced state word u64Z.
 */
static uint64_t
b3723_mix(uint64_t u64Z)
{
	u64Z = (u64Z ^ (u64Z >> 30)) * B3723_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B3723_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_splitmix64_u — one reentrant SplitMix64 step.
 *
 * state: mutable 64-bit generator state; advanced by B3723_GAMMA each call.
 *        NULL -> returns 0 without writing.
 *
 * Returns the mixed 64-bit output after advancing *state.
 */
uint64_t
gj_rng_splitmix64_u(uint64_t *state)
{
	uint64_t u64Z;

	(void)NULL;

	if (state == NULL) {
		return 0ULL;
	}

	u64Z = (*state += B3723_GAMMA);
	return b3723_mix(u64Z);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rng_splitmix64_u(uint64_t *state)
    __attribute__((alias("gj_rng_splitmix64_u")));
