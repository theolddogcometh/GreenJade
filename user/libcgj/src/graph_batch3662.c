/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3662: Bloom filter secondary hash (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bloom_hash2_u(const void *key, size_t n);
 *     - Second independent 64-bit digest for Kirsch–Mitzenmacher
 *       double-hashing. FNV-1a 64 with a distinct offset, mixed with
 *       the primary digest, then forced odd so (h1 + i*h2) has full
 *       period when the bit-count m is a power of two.
 *   uint64_t __gj_bloom_hash2_u  (alias)
 *   __libcgj_batch3662_marker = "libcgj-batch3662"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Companion to
 * gj_bloom_hash1_u (batch3661). Distinct from gj_bloom_* (batch125) —
 * unique _u hash2 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3662_marker[] = "libcgj-batch3662";

/* Distinct FNV bases / mix constants (static to this TU). */
#define B3662_FNV_OFF1   0xcbf29ce484222325ULL
#define B3662_FNV_OFF2   0x84222325cbf29ce4ULL
#define B3662_FNV_PRIME  0x100000001b3ULL
#define B3662_MIX        0x9e3779b97f4a7c15ULL
#define B3662_MIX2       0xff51afd7ed558ccdULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3662_fnv1a64(uint64_t uOff, const void *pKey, size_t cb)
{
	const unsigned char *p;
	uint64_t h;
	size_t i;

	h = uOff;
	if (pKey == NULL || cb == 0u) {
		return h;
	}
	p = (const unsigned char *)pKey;
	for (i = 0u; i < cb; i++) {
		h ^= (uint64_t)p[i];
		h *= B3662_FNV_PRIME;
	}
	return h;
}

/*
 * Secondary digest: FNV with alternate offset, mixed with primary,
 * forced odd for full period when m is a power of two.
 */
static uint64_t
b3662_hash2(const void *pKey, size_t cb)
{
	uint64_t h1;
	uint64_t h2;

	h1 = b3662_fnv1a64(B3662_FNV_OFF1, pKey, cb);
	h2 = b3662_fnv1a64(B3662_FNV_OFF2 ^ B3662_MIX, pKey, cb);
	h2 ^= h1 >> 33;
	h2 *= B3662_MIX2;
	h2 |= 1ULL;
	return h2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_hash2_u - secondary (odd) Bloom / double-hash digest.
 *
 * key/n: byte string; NULL or n == 0 still yields a defined odd value.
 * Returns an odd 64-bit digest. Does not call libc. No parent wires.
 */
uint64_t
gj_bloom_hash2_u(const void *pKey, size_t cN)
{
	(void)NULL;
	return b3662_hash2(pKey, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bloom_hash2_u(const void *pKey, size_t cN)
    __attribute__((alias("gj_bloom_hash2_u")));
