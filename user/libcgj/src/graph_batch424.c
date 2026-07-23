/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch424: SplitMix64 pure one-shot mixer.
 *
 * Surface (unique symbols only — no shared helper export from batch302):
 *   uint64_t gj_splitmix64(uint64_t x);
 *     — Pure SplitMix64 mix of a single 64-bit value (no mutable state).
 *       Adds the golden-ratio constant, then two multiply-xor finalizers.
 *   __gj_splitmix64  (alias)
 *   __libcgj_batch424_marker = "libcgj-batch424"
 *
 * Distinct from batch302 file-static b302_splitmix64 (not exported).
 * This unit only defines gj_splitmix64 and its alias.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Constants match the public SplitMix64
 * algorithm (Steele / Vigna lineage).
 */

#include <stdint.h>

const char __libcgj_batch424_marker[] = "libcgj-batch424";

/* SplitMix64 public algorithm constants. */
#define B424_GAMMA  0x9e3779b97f4a7c15ULL
#define B424_M1     0xbf58476d1ce4e5b9ULL
#define B424_M2     0x94d049bb133111ebULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splitmix64 — pure one-shot SplitMix64 mix of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns the mixed 64-bit output. Stateless: does not retain seed.
 */
uint64_t
gj_splitmix64(uint64_t x)
{
	uint64_t u64Z;

	u64Z = x + B424_GAMMA;
	u64Z = (u64Z ^ (u64Z >> 30)) * B424_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B424_M2;
	return u64Z ^ (u64Z >> 31);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_splitmix64(uint64_t x)
    __attribute__((alias("gj_splitmix64")));
