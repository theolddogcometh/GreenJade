/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2184: reentrant SplitMix64 step (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_splitmix64_next_u(uint64_t *state);
 *     - One SplitMix64 step: advance *state by the golden-ratio gamma,
 *       then apply the two multiply-xor finalizers. Returns the mixed
 *       64-bit output. state == NULL -> 0 (no write).
 *   uint64_t __gj_splitmix64_next_u  (alias)
 *   __libcgj_batch2184_marker = "libcgj-batch2184"
 *
 * Distinct from gj_splitmix64 (batch424 pure one-shot) and
 * gj_splitmix64_next (batch714). Unique _u export for the post-2180
 * random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public
 * SplitMix64 algorithm (Steele / Vigna lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2184_marker[] = "libcgj-batch2184";

/* SplitMix64 public algorithm constants. */
#define B2184_GAMMA  0x9e3779b97f4a7c15ULL
#define B2184_M1     0xbf58476d1ce4e5b9ULL
#define B2184_M2     0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Avalanche finalizer on a raw advanced state word z. */
static uint64_t
b2184_mix(uint64_t u64Z)
{
	u64Z = (u64Z ^ (u64Z >> 30)) * B2184_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B2184_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splitmix64_next_u - one stateful SplitMix64 step (_u surface).
 *
 * state: mutable 64-bit generator state; advanced by B2184_GAMMA each call.
 *        NULL -> returns 0 without writing.
 *
 * Returns the mixed 64-bit output after advancing *state.
 */
uint64_t
gj_splitmix64_next_u(uint64_t *pState)
{
	uint64_t u64Z;

	(void)NULL;

	if (pState == NULL) {
		return 0ULL;
	}

	u64Z = (*pState += B2184_GAMMA);
	return b2184_mix(u64Z);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_splitmix64_next_u(uint64_t *pState)
    __attribute__((alias("gj_splitmix64_next_u")));
