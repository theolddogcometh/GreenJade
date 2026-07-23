/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2185: wyhash-family seed step (PRNG-style).
 *
 * Surface (unique symbols):
 *   uint64_t gj_wyhash64_seed_step(uint64_t *seed, uint64_t ent);
 *     - Advance a caller-owned seed with one wyhash-inspired mum mix of
 *       (*seed ^ p0) against (ent ^ p1), write the new seed, and return
 *       it. NULL seed -> 0 (no write). Suitable as a non-crypto stream
 *       mixer / seed expander (not a full buffer hash).
 *   uint64_t __gj_wyhash64_seed_step  (alias)
 *   __libcgj_batch2185_marker = "libcgj-batch2185"
 *
 * Distinct from gj_wyhash64 (batch270 one-shot buffer hash). Unique
 * seed_step export for the post-2180 random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __uint128_t. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2185_marker[] = "libcgj-batch2185";

/* wyhash-family public odd mix primes (algorithm-public constants). */
#define B2185_P0 0xa0761d6478bd642fULL
#define B2185_P1 0xe7037ed1a0b428dbULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b2185_mum - 64x64 -> 128 multiply, fold high ^ low (wyhash-style mum).
 * Portable split multiply; no __uint128_t.
 */
static uint64_t
b2185_mum(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Alo = u64A & 0xffffffffULL;
	uint64_t u64Ahi = u64A >> 32;
	uint64_t u64Blo = u64B & 0xffffffffULL;
	uint64_t u64Bhi = u64B >> 32;
	uint64_t u64LoLo = u64Alo * u64Blo;
	uint64_t u64HiLo = u64Ahi * u64Blo;
	uint64_t u64LoHi = u64Alo * u64Bhi;
	uint64_t u64HiHi = u64Ahi * u64Bhi;
	uint64_t u64Cross = (u64LoLo >> 32) + (u64HiLo & 0xffffffffULL) +
	                    u64LoHi;
	uint64_t u64Upper = (u64HiLo >> 32) + (u64Cross >> 32) + u64HiHi;
	uint64_t u64Lower = (u64Cross << 32) | (u64LoLo & 0xffffffffULL);

	return u64Upper ^ u64Lower;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wyhash64_seed_step - mix entropy into a mutable seed and return it.
 *
 * seed: caller-owned 64-bit seed word (updated in place)
 * ent:  entropy / input lane mixed against the seed
 *
 * Computes mum(seed ^ p0, ent ^ p1), stores into *seed, returns new seed.
 * NULL seed -> 0.
 */
uint64_t
gj_wyhash64_seed_step(uint64_t *pSeed, uint64_t u64Ent)
{
	uint64_t u64Out;

	(void)NULL;

	if (pSeed == NULL) {
		return 0ULL;
	}

	u64Out = b2185_mum(*pSeed ^ B2185_P0, u64Ent ^ B2185_P1);
	*pSeed = u64Out;
	return u64Out;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_wyhash64_seed_step(uint64_t *pSeed, uint64_t u64Ent)
    __attribute__((alias("gj_wyhash64_seed_step")));
