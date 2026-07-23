/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1441: wyhash-family 64-bit finalizer mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_wyhash64_mix(uint64_t x);
 *     — Pure single-word finalizer inspired by Wang Yi wyhash mum/mix
 *       finish: fold x against public wyp-class primes via portable
 *       64×64→128 mum (high ^ low). No buffer walk; no seed state.
 *   uint64_t __gj_wyhash64_mix  (alias)
 *   __libcgj_batch1441_marker = "libcgj-batch1441"
 *
 * Distinct from gj_wyhash64 buffer hash (batch270), gj_hash_mix64 /
 * gj_murmur_fmix64, gj_hash_finalizer64, and gj_splitmix64. Unique
 * surface only; no multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __uint128_t. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1441_marker[] = "libcgj-batch1441";

/* Public odd wyhash-family mix primes (algorithm-public constants). */
#define B1441_P0 0xa0761d6478bd642fULL
#define B1441_P1 0xe7037ed1a0b428dbULL
#define B1441_P2 0x8ebc6af09c88c6e3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1441_mum — 64×64 → 128 multiply, fold high ^ low (wyhash-style mum).
 * Portable split multiply; no __uint128_t.
 */
static uint64_t
b1441_mum(uint64_t u64A, uint64_t u64B)
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
 * gj_wyhash64_mix — finalizer mix of a single 64-bit word.
 *
 * u64X: arbitrary input (all values accepted, including 0).
 *
 * Two mum stages: first against (x^p0, x^p1), then self-mix with p2.
 * Stateless pure function.
 */
uint64_t
gj_wyhash64_mix(uint64_t u64X)
{
	uint64_t u64A;

	u64A = b1441_mum(u64X ^ B1441_P0, u64X ^ B1441_P1);
	return b1441_mum(u64A, u64A ^ B1441_P2);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_wyhash64_mix(uint64_t u64X)
    __attribute__((alias("gj_wyhash64_mix")));
