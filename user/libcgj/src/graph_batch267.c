/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch267: CityHash64-style buffer mix hash.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cityhash64(const void *data, size_t n);
 *     — one-shot 64-bit hash of the n-byte buffer at data.
 *       data may be NULL only when n == 0 (empty → fixed seed K2).
 *       data == NULL && n > 0 → 0.
 *   __gj_cityhash64  (alias)
 *   __libcgj_batch267_marker = "libcgj-batch267"
 *
 * Algorithm (clean-room pure mix in the CityHash / FarmHash continuum;
 * not bitwise-identical to Google CityHash64 — that surface lives as
 * cityhash64 in batch61). Documented constants and steps:
 *
 *   Constants (public CityHash primes + Hash128to64 mul):
 *     K0 = 0xc3a5c85c97cb3127  — odd prime mixer A
 *     K1 = 0xb492b66fbe98f273  — odd prime mixer B
 *     K2 = 0x9ae16a3b2f90404f  — length / empty seed
 *     KM = 0x9ddfea08eb382d69  — final 128→64 mul (Murmur-inspired)
 *
 *   1. Seed: h = n * K2; x = K0 ^ (n * K1)  (dual accumulators).
 *   2. Full 16-byte blocks (LE fetch): for each (a,b) pair:
 *        h = rotr64(h + a * K0, 27) * K1 ^ x
 *        x = rotr64(x + b * K1, 33) * K0 ^ h
 *   3. Tail 8-byte LE word (if any): h = (h ^ w) * K1; h ^= h >> 47.
 *   4. Residual 1..7 bytes as partial LE word mixed with K0.
 *   5. Avalanche: h ^= x; h = shiftmix(h) * KM; h = shiftmix(h).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from batch61 cityhash64.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch267_marker[] = "libcgj-batch267";

/* CityHash / FarmHash family primes (public algorithm constants). */
#define B267_K0  0xc3a5c85c97cb3127ull
#define B267_K1  0xb492b66fbe98f273ull
#define B267_K2  0x9ae16a3b2f90404full
#define B267_KM  0x9ddfea08eb382d69ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Right-rotate (CityHash Rotate). n is reduced mod 64. */
static uint64_t
b267_rotr64(uint64_t x, unsigned n)
{
	n &= 63u;
	if (n == 0u) {
		return x;
	}
	return (x >> n) | (x << (64u - n));
}

/* Shift-mix avalanche step (x ^ (x >> 47)). */
static uint64_t
b267_shiftmix(uint64_t v)
{
	return v ^ (v >> 47);
}

/* Little-endian unaligned 64-bit load from byte pointer. */
static uint64_t
b267_fetch64(const unsigned char *p)
{
	return (uint64_t)p[0]
	    | ((uint64_t)p[1] << 8)
	    | ((uint64_t)p[2] << 16)
	    | ((uint64_t)p[3] << 24)
	    | ((uint64_t)p[4] << 32)
	    | ((uint64_t)p[5] << 40)
	    | ((uint64_t)p[6] << 48)
	    | ((uint64_t)p[7] << 56);
}

/*
 * Fetch up to 7 residual bytes as a little-endian partial word.
 * len must be in 1..7; does not read past p[len-1].
 */
static uint64_t
b267_fetch_tail(const unsigned char *p, size_t len)
{
	uint64_t w = 0ull;
	size_t i;

	for (i = 0u; i < len; i++) {
		w |= (uint64_t)p[i] << (i * 8u);
	}
	return w;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cityhash64 — CityHash-style 64-bit pure mix over a byte buffer.
 *
 * Special cases:
 *   n == 0                → K2  (empty-buffer seed; matches family empty)
 *   data == NULL && n > 0 → 0
 *
 * Otherwise hashes exactly n bytes starting at data (unaligned-safe).
 */
uint64_t
gj_cityhash64(const void *data, size_t n)
{
	const unsigned char *p;
	uint64_t h;
	uint64_t x;
	size_t nLeft;

	if (n == 0u) {
		return B267_K2;
	}
	if (data == NULL) {
		return 0ull;
	}

	p = (const unsigned char *)data;
	h = (uint64_t)n * B267_K2;
	x = B267_K0 ^ ((uint64_t)n * B267_K1);
	nLeft = n;

	/* 16-byte blocks: dual-accumulator FarmHash-lite body. */
	while (nLeft >= 16u) {
		uint64_t a = b267_fetch64(p);
		uint64_t b = b267_fetch64(p + 8);

		h = (b267_rotr64(h + a * B267_K0, 27) * B267_K1) ^ x;
		x = (b267_rotr64(x + b * B267_K1, 33) * B267_K0) ^ h;
		p += 16;
		nLeft -= 16u;
	}

	/* Optional full 8-byte tail word. */
	if (nLeft >= 8u) {
		uint64_t w = b267_fetch64(p);

		h = (h ^ w) * B267_K1;
		h = b267_shiftmix(h);
		p += 8;
		nLeft -= 8u;
	}

	/* Residual 1..7 bytes. */
	if (nLeft > 0u) {
		uint64_t w = b267_fetch_tail(p, nLeft);

		h = (h + w) * B267_K0;
		h = b267_shiftmix(h);
	}

	/* Final combine + avalanche. */
	h ^= x;
	h = b267_shiftmix(h) * B267_KM;
	h = b267_shiftmix(h);
	return h;
}

uint64_t __gj_cityhash64(const void *data, size_t n)
    __attribute__((alias("gj_cityhash64")));
