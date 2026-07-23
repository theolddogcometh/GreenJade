/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4231: classic SplitMix64 stateful next step.
 *
 * Surface (unique symbols):
 *   uint64_t gj_splitmix64_next_u2(uint64_t *state);
 *     - One SplitMix64 step: advance *state by the golden-ratio gamma,
 *       then apply the two multiply-xor finalizers. Returns the mixed
 *       64-bit output. state == NULL -> 0 (no write).
 *   uint64_t __gj_splitmix64_next_u2  (alias)
 *   __libcgj_batch4231_marker = "libcgj-batch4231"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_splitmix64_next_u2 surface
 * only; no multi-def of other symbols in this TU. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * SplitMix64 algorithm (Steele / Vigna lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4231_marker[] = "libcgj-batch4231";

/* SplitMix64 public algorithm constants. */
#define B4231_GAMMA  0x9e3779b97f4a7c15ULL
#define B4231_M1     0xbf58476d1ce4e5b9ULL
#define B4231_M2     0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Avalanche finalizer on a raw advanced state word z. */
static uint64_t
b4231_mix(uint64_t u64Z)
{
	u64Z = (u64Z ^ (u64Z >> 30)) * B4231_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B4231_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splitmix64_next_u2 - one stateful SplitMix64 step.
 *
 * state: mutable 64-bit generator state; advanced by B4231_GAMMA each call.
 *        NULL -> returns 0 without writing.
 *
 * Returns the mixed 64-bit output after advancing *state.
 * No parent wires.
 */
uint64_t
gj_splitmix64_next_u2(uint64_t *state)
{
	uint64_t u64Z;

	(void)NULL;

	if (state == NULL) {
		return 0ULL;
	}

	u64Z = (*state += B4231_GAMMA);
	return b4231_mix(u64Z);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_splitmix64_next_u2(uint64_t *state)
    __attribute__((alias("gj_splitmix64_next_u2")));
