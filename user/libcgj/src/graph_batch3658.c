/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3658: gj_wyhash_mix_u - wyhash-family mum mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_wyhash_mix_u(uint64_t x);
 *     - Pure single-word wyhash-style mum mix: fold x against public
 *       wyp-class primes via portable 64×64→128 mum (high ^ low), then
 *       self-mix. No buffer walk; no __int128.
 *   uint64_t __gj_wyhash_mix_u  (alias)
 *   __libcgj_batch3658_marker = "libcgj-batch3658"
 *
 * Hash exclusive wave (3651-3660). Distinct from gj_wyhash64 (batch270
 * buffer hash) and gj_wyhash64_mix (batch1441) — unique _mix_u name only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __uint128_t. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3658_marker[] = "libcgj-batch3658";

/* Public odd wyhash-family mix primes (algorithm-public constants). */
#define B3658_P0 0xa0761d6478bd642fULL
#define B3658_P1 0xe7037ed1a0b428dbULL
#define B3658_P2 0x8ebc6af09c88c6e3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b3658_mum — 64×64 → 128 multiply, fold high ^ low (wyhash-style mum).
 * Portable split multiply; no __uint128_t / no __int128.
 */
static uint64_t
b3658_mum(uint64_t u64A, uint64_t u64B)
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

static uint64_t
b3658_mix(uint64_t u64X)
{
	uint64_t u64A;

	u64A = b3658_mum(u64X ^ B3658_P0, u64X ^ B3658_P1);
	return b3658_mum(u64A, u64A ^ B3658_P2);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wyhash_mix_u - wyhash-style mum finalizer of a single 64-bit word.
 *
 * x: arbitrary input (all values accepted, including 0)
 *
 * Two mum stages against public wyp primes. Stateless pure function.
 */
uint64_t
gj_wyhash_mix_u(uint64_t u64X)
{
	(void)NULL;
	return b3658_mix(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_wyhash_mix_u(uint64_t u64X)
    __attribute__((alias("gj_wyhash_mix_u")));
