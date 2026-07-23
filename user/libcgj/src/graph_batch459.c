/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch459: FarmHash-inspired 64-bit one-shot mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_farmhash64(const void *data, size_t n);
 *     — freestanding 64-bit mix hash of the n-byte buffer at data.
 *       FarmHash-inspired pure mix (Google FarmHash / CityHash continuum
 *       public primes + rotate / mul / shiftmix structure). Clean-room
 *       reimplementation — not bitwise-identical to any Google FarmHash
 *       release. data may be NULL only when n == 0 (empty → fixed seed K2).
 *       data == NULL && n > 0 → 0.
 *   __gj_farmhash64  (alias)
 *   __libcgj_batch459_marker = "libcgj-batch459"
 *
 * Algorithm (documented FarmHash-inspired mix steps):
 *
 *   Constants (public FarmHash / CityHash family odd primes):
 *     K0 = 0xc3a5c85c97cb3127  — odd prime mixer A
 *     K1 = 0xb492b66fbe98f273  — odd prime mixer B
 *     K2 = 0x9ae16a3b2f90404f  — length / empty seed
 *     KM = 0x9ddfea08eb382d69  — final 128→64-style mul
 *
 *   1. Seed: u64H = n * K2; u64X = K0 ^ (n * K1)  (dual accumulators).
 *   2. Full 16-byte blocks (LE fetch): for each (u64A, u64B) pair:
 *        u64H = rotr64(u64H + u64A * K0, 29) * K1 ^ u64X
 *        u64X = rotr64(u64X + u64B * K1, 37) * K0 ^ u64H
 *      (rotate distances differ from cityhash-style siblings — farmhash-flavored.)
 *   3. Tail 8-byte LE word (if any): u64H = (u64H ^ w) * K1; shiftmix.
 *   4. Residual 1..7 bytes as partial LE word mixed with K0.
 *   5. Avalanche: u64H ^= u64X; u64H = shiftmix(u64H) * KM; shiftmix.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Distinct from batch61 cityhash64
 * and batch267 gj_cityhash64.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch459_marker[] = "libcgj-batch459";

/* FarmHash / CityHash family primes (public algorithm constants). */
#define B459_K0  0xc3a5c85c97cb3127ull
#define B459_K1  0xb492b66fbe98f273ull
#define B459_K2  0x9ae16a3b2f90404full
#define B459_KM  0x9ddfea08eb382d69ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Right-rotate; uN is reduced mod 64. */
static uint64_t
b459_rotr64(uint64_t u64X, unsigned uN)
{
	uN &= 63u;
	if (uN == 0u) {
		return u64X;
	}
	return (u64X >> uN) | (u64X << (64u - uN));
}

/* Shift-mix avalanche step (x ^ (x >> 47)). */
static uint64_t
b459_shiftmix(uint64_t u64V)
{
	return u64V ^ (u64V >> 47);
}

/* Little-endian unaligned 64-bit load from byte pointer. */
static uint64_t
b459_fetch64(const unsigned char *p)
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
 * cb must be in 1..7; does not read past p[cb-1].
 */
static uint64_t
b459_fetch_tail(const unsigned char *p, size_t cb)
{
	uint64_t u64W = 0ull;
	size_t iByte;

	for (iByte = 0u; iByte < cb; iByte++) {
		u64W |= (uint64_t)p[iByte] << (iByte * 8u);
	}
	return u64W;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_farmhash64 — FarmHash-inspired 64-bit pure mix over a byte buffer.
 *
 * Special cases:
 *   n == 0                → K2  (empty-buffer seed; matches family empty)
 *   data == NULL && n > 0 → 0
 *
 * Otherwise hashes exactly n bytes starting at data (unaligned-safe).
 * Deterministic freestanding mix; not bitwise-identical to Google FarmHash.
 */
uint64_t
gj_farmhash64(const void *data, size_t n)
{
	const unsigned char *p;
	uint64_t u64H;
	uint64_t u64X;
	size_t cbLeft;

	if (n == 0u) {
		return B459_K2;
	}
	if (data == NULL) {
		return 0ull;
	}

	p = (const unsigned char *)data;
	u64H = (uint64_t)n * B459_K2;
	u64X = B459_K0 ^ ((uint64_t)n * B459_K1);
	cbLeft = n;

	/* 16-byte blocks: dual-accumulator FarmHash-inspired body. */
	while (cbLeft >= 16u) {
		uint64_t u64A = b459_fetch64(p);
		uint64_t u64B = b459_fetch64(p + 8);

		u64H = (b459_rotr64(u64H + u64A * B459_K0, 29) * B459_K1) ^ u64X;
		u64X = (b459_rotr64(u64X + u64B * B459_K1, 37) * B459_K0) ^ u64H;
		p += 16;
		cbLeft -= 16u;
	}

	/* Optional full 8-byte tail word. */
	if (cbLeft >= 8u) {
		uint64_t u64W = b459_fetch64(p);

		u64H = (u64H ^ u64W) * B459_K1;
		u64H = b459_shiftmix(u64H);
		p += 8;
		cbLeft -= 8u;
	}

	/* Residual 1..7 bytes. */
	if (cbLeft > 0u) {
		uint64_t u64W = b459_fetch_tail(p, cbLeft);

		u64H = (u64H + u64W) * B459_K0;
		u64H = b459_shiftmix(u64H);
	}

	/* Final combine + avalanche. */
	u64H ^= u64X;
	u64H = b459_shiftmix(u64H) * B459_KM;
	u64H = b459_shiftmix(u64H);
	return u64H;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_farmhash64(const void *data, size_t n)
    __attribute__((alias("gj_farmhash64")));
