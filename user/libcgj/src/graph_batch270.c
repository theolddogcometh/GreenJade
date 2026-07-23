/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch270: wyhash-inspired 64-bit one-shot hash.
 *
 * Surface (unique symbols):
 *   uint64_t gj_wyhash64(const void *data, size_t n, uint64_t seed);
 *     — freestanding 64-bit mix hash of data[0..n) with seed.
 *       Structure inspired by Wang Yi's wyhash (mum / mix / LE loads),
 *       clean-room reimplementation — not bitwise-identical to any
 *       upstream wyhash release. NULL data with n != 0 → 0; empty n==0
 *       is valid and seed-dependent.
 *   __gj_wyhash64  (alias)
 *   __libcgj_batch270_marker = "libcgj-batch270"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no __uint128_t / no libgcc __multi3. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch270_marker[] = "libcgj-batch270";

/*
 * Public odd 64-bit mix primes (wyhash-family _wyp-class constants;
 * algorithm-public values, low bit set so multiplies stay invertible mod 2^64).
 */
#define B270_P0 0xa0761d6478bd642full
#define B270_P1 0xe7037ed1a0b428dbull
#define B270_P2 0x8ebc6af09c88c6e3ull
#define B270_P3 0x589965cc75374cc3ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Little-endian 32-bit load from arbitrary alignment (byte path). */
static uint32_t
b270_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Little-endian 64-bit load from arbitrary alignment (byte path). */
static uint64_t
b270_read64le(const unsigned char *p)
{
	return (uint64_t)b270_read32le(p) |
	       ((uint64_t)b270_read32le(p + 4) << 32);
}

/*
 * b270_mum — 64×64 → 128 multiply, fold high ^ low (wyhash-style mum).
 * Portable split multiply; no __uint128_t (freestanding / no libgcc multi3).
 */
static uint64_t
b270_mum(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Alo = u64A & 0xffffffffull;
	uint64_t u64Ahi = u64A >> 32;
	uint64_t u64Blo = u64B & 0xffffffffull;
	uint64_t u64Bhi = u64B >> 32;
	uint64_t u64LoLo = u64Alo * u64Blo;
	uint64_t u64HiLo = u64Ahi * u64Blo;
	uint64_t u64LoHi = u64Alo * u64Bhi;
	uint64_t u64HiHi = u64Ahi * u64Bhi;
	uint64_t u64Cross = (u64LoLo >> 32) + (u64HiLo & 0xffffffffull) + u64LoHi;
	uint64_t u64Upper = (u64HiLo >> 32) + (u64Cross >> 32) + u64HiHi;
	uint64_t u64Lower = (u64Cross << 32) | (u64LoLo & 0xffffffffull);

	return u64Upper ^ u64Lower;
}

/* b270_mix — one mum fold of (a ^ p) against (b ^ q) style operands. */
static uint64_t
b270_mix(uint64_t u64A, uint64_t u64B)
{
	return b270_mum(u64A, u64B);
}

/*
 * b270_r3 — short tail of 1..3 bytes, little-endian packed into a u64.
 * nLen in 1..3; caller guarantees bounds.
 */
static uint64_t
b270_r3(const unsigned char *p, size_t nLen)
{
	uint64_t u64V = (uint64_t)p[0];

	if (nLen >= 2u) {
		u64V |= (uint64_t)p[1] << 8;
	}
	if (nLen >= 3u) {
		u64V |= (uint64_t)p[2] << 16;
	}
	return u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wyhash64 — one-shot 64-bit wyhash-inspired mix.
 *
 * data: byte buffer (NULL only legal when n == 0).
 * n:    length in bytes.
 * seed: caller seed mixed into the state.
 *
 * Algorithm sketch:
 *   seed' = seed ^ B270_P0
 *   consume 16-byte blocks: seed = mix(a ^ seed, b ^ secrets)
 *   tail via length-class mixes, then final mum with length.
 * Deterministic, freestanding, no table.
 * NULL data with n != 0 → 0 (defensive, matches sibling hash batches).
 */
uint64_t
gj_wyhash64(const void *data, size_t n, uint64_t seed)
{
	const unsigned char *p;
	uint64_t u64Seed;
	uint64_t u64A;
	uint64_t u64B;
	size_t cb;

	p = (const unsigned char *)data;
	if (p == NULL && n != 0u) {
		return 0ull;
	}

	u64Seed = seed ^ B270_P0;
	cb = n;

	/* Bulk: 16-byte stripes. */
	while (cb >= 16u) {
		u64A = b270_read64le(p) ^ B270_P1;
		u64B = b270_read64le(p + 8) ^ B270_P2;
		u64Seed = b270_mix(u64A ^ u64Seed, u64B);
		p += 16u;
		cb -= 16u;
	}

	/* Tail 0..15. */
	u64A = 0u;
	u64B = 0u;
	if (cb >= 8u) {
		u64A = b270_read64le(p);
		p += 8u;
		cb -= 8u;
	}
	if (cb >= 4u) {
		u64B = (uint64_t)b270_read32le(p);
		p += 4u;
		cb -= 4u;
	}
	if (cb > 0u) {
		u64B |= b270_r3(p, cb) << 32;
	}

	/* Full length mixed into second lane (wyhash-like finish). */
	u64B ^= (uint64_t)n ^ B270_P3;
	u64Seed = b270_mix(u64A ^ u64Seed, u64B);

	/* Final avalanche. */
	return b270_mix(u64Seed, u64Seed ^ B270_P0);
}

uint64_t __gj_wyhash64(const void *data, size_t n, uint64_t seed)
    __attribute__((alias("gj_wyhash64")));
