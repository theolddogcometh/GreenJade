/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch125: classic Bloom filter.
 * Clean-room freestanding pure C — builds under userspace -msse2.
 * No third-party Bloom / hash source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   fnv1a_32 / fnv1a_64 / __fnv1a_*           → graph_batch41.c
 *   Murmur / SipHash / XXH*                  → batch36 / 40 / 41 / 98
 *
 * This TU adds only unique symbols:
 *   void gj_bloom_init(unsigned char *bits, size_t nbytes);
 *   void gj_bloom_add(unsigned char *bits, size_t nbytes,
 *                     const void *key, size_t klen, unsigned k_hashes);
 *   int  gj_bloom_may_contain(const unsigned char *bits, size_t nbytes,
 *                             const void *key, size_t klen,
 *                             unsigned k_hashes);
 *   __gj_bloom_init / __gj_bloom_add / __gj_bloom_may_contain  (aliases)
 *   __libcgj_batch125_marker = "libcgj-batch125"
 *
 * Hashing: Kirsch–Mitzenmacher double-hashing with two independent
 * inline FNV-1a 64-bit digests (static to this TU — not the batch41
 * exports). Bit positions:
 *   m  = nbytes * 8
 *   h_i = (h1 + i * h2) mod m   for i in [0, k_hashes)
 * h2 is forced odd so the linear sequence covers a full period when
 * m is a power of two.
 *
 * Semantics:
 *   init: zero the bit array (no-op if bits==NULL or nbytes==0).
 *   add: set k_hashes bit positions for key (no-op on bad args).
 *   may_contain: 1 if all k_hashes bits are set (possible member),
 *                0 if any bit is clear (definite non-member) or bad args.
 *                k_hashes==0 → 1 (vacuous membership).
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

const char __libcgj_batch125_marker[] = "libcgj-batch125";

/* FNV-1a 64 offset bases (distinct pair for double-hashing). */
#define B125_FNV_OFF1  0xcbf29ce484222325ULL /* standard FNV-1a 64 offset */
#define B125_FNV_OFF2  0x84222325cbf29ce4ULL /* distinct second basis */
#define B125_FNV_PRIME 0x100000001b3ULL

/* ---- freestanding helpers ---------------------------------------------- */

static void
b125_zero(unsigned char *pBits, size_t cb)
{
	size_t i = 0;

	if (pBits == NULL || cb == 0u) {
		return;
	}

#if defined(__x86_64__) || defined(__i386__)
	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;
		__m128i vZ = _mm_setzero_si128();

		while (i < nBlk) {
			_mm_storeu_si128((__m128i *)(void *)(pBits + i), vZ);
			i += 16u;
		}
	}
#endif
	while (i < cb) {
		pBits[i] = 0u;
		i++;
	}
}

/*
 * Inline FNV-1a 64 with caller-chosen offset basis. Static — does not
 * collide with batch41 fnv1a_64.
 */
static uint64_t
b125_fnv1a64(uint64_t uOff, const void *pKey, size_t cb)
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
		h *= B125_FNV_PRIME;
	}
	return h;
}

/* Two independent 64-bit hashes for Kirsch–Mitzenmacher. */
static void
b125_hashes(const void *pKey, size_t cb, uint64_t *pH1, uint64_t *pH2)
{
	uint64_t h1;
	uint64_t h2;

	/*
	 * Second digest: FNV-1a over the same key but with a distinct
	 * offset, then folded with the first so empty keys still differ.
	 */
	h1 = b125_fnv1a64(B125_FNV_OFF1, pKey, cb);
	h2 = b125_fnv1a64(B125_FNV_OFF2 ^ 0x9e3779b97f4a7c15ULL, pKey, cb);
	h2 ^= h1 >> 33;
	h2 *= 0xff51afd7ed558ccdULL;
	/* Force odd: full period when m is a power of two. */
	h2 |= 1ULL;

	*pH1 = h1;
	*pH2 = h2;
}

static size_t
b125_bit_index(uint64_t h1, uint64_t h2, unsigned iHash, size_t nBits)
{
	uint64_t uIdx;

	/* (h1 + i * h2) mod m  — m may not be power of two. */
	uIdx = h1 + (uint64_t)iHash * h2;
	return (size_t)(uIdx % (uint64_t)nBits);
}

static void
b125_set_bit(unsigned char *pBits, size_t uBit)
{
	pBits[uBit >> 3] |= (unsigned char)(1u << (uBit & 7u));
}

static int
b125_test_bit(const unsigned char *pBits, size_t uBit)
{
	return (pBits[uBit >> 3] & (unsigned char)(1u << (uBit & 7u))) != 0;
}

/* ---- public API -------------------------------------------------------- */

void
gj_bloom_init(unsigned char *bits, size_t nbytes)
{
	b125_zero(bits, nbytes);
}

void
gj_bloom_add(unsigned char *bits, size_t nbytes, const void *key, size_t klen,
             unsigned k_hashes)
{
	uint64_t h1;
	uint64_t h2;
	size_t nBits;
	unsigned i;

	if (bits == NULL || nbytes == 0u || k_hashes == 0u) {
		return;
	}
	if (key == NULL && klen != 0u) {
		return;
	}

	nBits = nbytes * 8u;
	b125_hashes(key, klen, &h1, &h2);

	for (i = 0u; i < k_hashes; i++) {
		size_t uBit = b125_bit_index(h1, h2, i, nBits);

		b125_set_bit(bits, uBit);
	}
}

int
gj_bloom_may_contain(const unsigned char *bits, size_t nbytes, const void *key,
                     size_t klen, unsigned k_hashes)
{
	uint64_t h1;
	uint64_t h2;
	size_t nBits;
	unsigned i;

	if (bits == NULL || nbytes == 0u) {
		return 0;
	}
	if (k_hashes == 0u) {
		return 1; /* vacuous: no probes required */
	}
	if (key == NULL && klen != 0u) {
		return 0;
	}

	nBits = nbytes * 8u;
	b125_hashes(key, klen, &h1, &h2);

	for (i = 0u; i < k_hashes; i++) {
		size_t uBit = b125_bit_index(h1, h2, i, nBits);

		if (!b125_test_bit(bits, uBit)) {
			return 0;
		}
	}
	return 1;
}

void __gj_bloom_init(unsigned char *bits, size_t nbytes)
    __attribute__((alias("gj_bloom_init")));
void __gj_bloom_add(unsigned char *bits, size_t nbytes, const void *key,
                    size_t klen, unsigned k_hashes)
    __attribute__((alias("gj_bloom_add")));
int __gj_bloom_may_contain(const unsigned char *bits, size_t nbytes,
                           const void *key, size_t klen, unsigned k_hashes)
    __attribute__((alias("gj_bloom_may_contain")));
