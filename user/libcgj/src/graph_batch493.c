/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch493: fixed 256-bit (four-word) Bloom filter.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party Bloom source was copied.
 *
 * Distinct from earlier Bloom surfaces (NOT redefined — avoid multi-def):
 *   gj_bloom_init / gj_bloom_add / gj_bloom_may_contain  → graph_batch125.c
 *   gj_bloom64_add / gj_bloom64_may                       → graph_batch208.c
 *   gj_bloom128_add / gj_bloom128_may                     → graph_batch349.c
 * This TU operates on a fixed m = 256 bit array as uint64_t bits[4] and
 * hashes raw key bytes (not a precomputed single-word hash).
 *
 * Surface (unique symbols):
 *   void gj_bloom256_add(uint64_t bits[4], const void *key, size_t n);
 *     — OR k bit positions for key into bits[0..3]. No-op if bits == NULL.
 *   int  gj_bloom256_may(const uint64_t bits[4], const void *key, size_t n);
 *     — 1 if all k bits for key are set (possible member), else 0.
 *   __gj_bloom256_add / __gj_bloom256_may  (aliases)
 *   __libcgj_batch493_marker = "libcgj-batch493"
 *
 * Hashing: Kirsch–Mitzenmacher double-hashing with two independent
 * inline FNV-1a 64-bit digests (static to this TU — not batch41/125/349
 * exports). Bit positions:
 *   m  = 256
 *   h_i = (h1 + i * h2) mod 256   for i in [0, k)
 * h2 is forced odd so the linear sequence covers a full period mod 256.
 * k = 4 probes (standard for m=256 at modest load).
 *
 * No errno / malloc (freestanding-safe). Callers zero bits[0..3]
 * before first use.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch493_marker[] = "libcgj-batch493";

/* Number of bit probes per key. */
#define B493_K 4u

/* Filter width in bits (four uint64_t words). */
#define B493_M 256u

/* FNV-1a 64 offset bases (distinct pair for double-hashing; TU-local). */
#define B493_FNV_OFF1  0xcbf29ce484222325ULL /* standard FNV-1a 64 offset */
#define B493_FNV_OFF2  0x9e3779b97f4a7c15ULL /* golden-ratio scramble basis */
#define B493_FNV_PRIME 0x100000001b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inline FNV-1a 64 with caller-chosen offset basis. Static — does not
 * collide with batch41 fnv1a_64 or batch125/349 helpers.
 */
static uint64_t
b493_fnv1a64(uint64_t uOff, const void *pKey, size_t cb)
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
		h *= B493_FNV_PRIME;
	}
	return h;
}

/* Two independent 64-bit hashes for Kirsch–Mitzenmacher. */
static void
b493_hashes(const void *pKey, size_t cb, uint64_t *pH1, uint64_t *pH2)
{
	uint64_t h1;
	uint64_t h2;

	h1 = b493_fnv1a64(B493_FNV_OFF1, pKey, cb);
	/*
	 * Second digest: FNV-1a with a distinct offset, then mixed with
	 * the first so empty / short keys still yield an independent step.
	 * Mix constants differ from batch349 so the 256-bit filter is not
	 * a pure rescaling of the 128-bit surface on the same key set.
	 */
	h2 = b493_fnv1a64(B493_FNV_OFF2 ^ 0x84222325cbf29ce4ULL, pKey, cb);
	h2 ^= h1 >> 33;
	h2 *= 0xff51afd7ed558ccdULL;
	/* Force odd: full period when m is a power of two. */
	h2 |= 1ULL;

	*pH1 = h1;
	*pH2 = h2;
}

/*
 * Set bit uBit (0..255) in the four-word filter.
 * Word w holds bits (w*64)..(w*64+63).
 */
static void
b493_set_bit(uint64_t bits[4], unsigned uBit)
{
	unsigned uWord;
	unsigned uOff;

	uWord = uBit >> 6;          /* / 64 */
	uOff = uBit & 63u;          /* % 64 */
	bits[uWord] |= (uint64_t)1ull << uOff;
}

/* Test bit uBit (0..255). Returns non-zero if set. */
static int
b493_test_bit(const uint64_t bits[4], unsigned uBit)
{
	unsigned uWord;
	unsigned uOff;

	uWord = uBit >> 6;
	uOff = uBit & 63u;
	return (bits[uWord] & ((uint64_t)1ull << uOff)) != 0ull;
}

/* ---- public API -------------------------------------------------------- */

/*
 * Insert key into the 256-bit filter by setting its k=4 probe bits.
 * No-op when bits is NULL. key may be NULL only when n == 0 (empty key).
 */
void
gj_bloom256_add(uint64_t bits[4], const void *key, size_t n)
{
	uint64_t h1;
	uint64_t h2;
	unsigned i;

	if (bits == NULL) {
		return;
	}
	if (key == NULL && n != 0u) {
		return;
	}

	b493_hashes(key, n, &h1, &h2);

	for (i = 0u; i < B493_K; i++) {
		/* (h1 + i * h2) mod 256 — m is power of two. */
		unsigned uBit = (unsigned)((h1 + (uint64_t)i * h2) &
		                           (uint64_t)(B493_M - 1u));

		b493_set_bit(bits, uBit);
	}
}

/*
 * Possible-membership test for key against the 256-bit filter.
 * Returns 1 if every probe bit is set, 0 if any is clear (definite miss)
 * or on bad args (bits == NULL, or key == NULL with n != 0).
 */
int
gj_bloom256_may(const uint64_t bits[4], const void *key, size_t n)
{
	uint64_t h1;
	uint64_t h2;
	unsigned i;

	if (bits == NULL) {
		return 0;
	}
	if (key == NULL && n != 0u) {
		return 0;
	}

	b493_hashes(key, n, &h1, &h2);

	for (i = 0u; i < B493_K; i++) {
		unsigned uBit = (unsigned)((h1 + (uint64_t)i * h2) &
		                           (uint64_t)(B493_M - 1u));

		if (!b493_test_bit(bits, uBit)) {
			return 0;
		}
	}
	return 1;
}

void __gj_bloom256_add(uint64_t bits[4], const void *key, size_t n)
    __attribute__((alias("gj_bloom256_add")));
int __gj_bloom256_may(const uint64_t bits[4], const void *key, size_t n)
    __attribute__((alias("gj_bloom256_may")));
