/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4232: SplitMix64 step, high 32-bit sample.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splitmix64_u32_u(uint64_t *state);
 *     - One classic SplitMix64 step on *state, then return the high
 *       32 bits of the mixed 64-bit output. state == NULL -> 0.
 *   uint32_t __gj_splitmix64_u32_u  (alias)
 *   __libcgj_batch4232_marker = "libcgj-batch4232"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_splitmix64_u32_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. SplitMix64 constants (Steele /
 * Vigna lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4232_marker[] = "libcgj-batch4232";

/* SplitMix64 public algorithm constants. */
#define B4232_GAMMA  0x9e3779b97f4a7c15ULL
#define B4232_M1     0xbf58476d1ce4e5b9ULL
#define B4232_M2     0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/* One SplitMix64 advance+mix; returns full 64-bit mixed word. */
static uint64_t
b4232_next(uint64_t *pState)
{
	uint64_t u64Z;

	u64Z = (*pState += B4232_GAMMA);
	u64Z = (u64Z ^ (u64Z >> 30)) * B4232_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B4232_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splitmix64_u32_u - SplitMix64 step, return high 32 bits.
 *
 * state: mutable 64-bit generator state (advanced by gamma each call).
 *        NULL -> returns 0 without writing.
 *
 * Returns (uint32_t)(mixed >> 32). No parent wires.
 */
uint32_t
gj_splitmix64_u32_u(uint64_t *state)
{
	uint64_t u64Mixed;

	(void)NULL;

	if (state == NULL) {
		return 0u;
	}

	u64Mixed = b4232_next(state);
	return (uint32_t)(u64Mixed >> 32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_splitmix64_u32_u(uint64_t *state)
    __attribute__((alias("gj_splitmix64_u32_u")));
