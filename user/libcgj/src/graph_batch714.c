/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch714: SplitMix64 stateful next step.
 *
 * Surface (unique symbols only — no multi-def of existing splitmix):
 *   uint64_t gj_splitmix64_next(uint64_t *state);
 *     — One SplitMix64 step: advance *state by the golden-ratio gamma,
 *       then apply the two multiply-xor finalizers. Returns the mixed
 *       64-bit output. state == NULL → 0 (no write).
 *   __gj_splitmix64_next  (alias)
 *   __libcgj_batch714_marker = "libcgj-batch714"
 *
 * Does NOT define gj_splitmix64 / __gj_splitmix64 (batch424 pure one-shot)
 * or batch302 file-static b302_splitmix64 (not exported). This unit only
 * defines gj_splitmix64_next and its alias — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Constants match the public SplitMix64
 * algorithm (Steele / Vigna lineage).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch714_marker[] = "libcgj-batch714";

/* SplitMix64 public algorithm constants. */
#define B714_GAMMA  0x9e3779b97f4a7c15ULL
#define B714_M1     0xbf58476d1ce4e5b9ULL
#define B714_M2     0x94d049bb133111ebULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splitmix64_next — one stateful SplitMix64 step.
 *
 * state: mutable 64-bit generator state; advanced by B714_GAMMA each call.
 *        NULL → returns 0 without writing.
 *
 * Returns the mixed 64-bit output after advancing *state. Distinct from
 * gj_splitmix64 (batch424), which is a pure one-shot mix of a value and
 * does not retain seed.
 */
uint64_t
gj_splitmix64_next(uint64_t *state)
{
	uint64_t u64Z;

	if (state == NULL) {
		return 0ULL;
	}

	u64Z = (*state += B714_GAMMA);
	u64Z = (u64Z ^ (u64Z >> 30)) * B714_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B714_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_splitmix64_next(uint64_t *state)
    __attribute__((alias("gj_splitmix64_next")));
