/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch98: XXH3-style 64/128-bit hash with SSE2
 * long-path accumulate. Clean-room freestanding C (-msse2). Userspace only.
 *
 * batch41 already owns XXH3_64bits / XXH3_64bits_withSeed, so this TU uses
 * unique names (no redefinition of the XXH3_* public surface):
 *   uint64_t gj_xxh3_64(const void *data, size_t len);
 *   void     gj_xxh3_128(const void *data, size_t len, uint64_t out[2]);
 *   __gj_xxh3_64 / __gj_xxh3_128  (aliases)
 *   __libcgj_batch98_marker = "libcgj-batch98"
 *
 * Design:
 *   - Short / mid lengths: scalar (matches XXH3 length-routed structure).
 *   - Long path (≥241 B): SSE2 stripe accumulate (64-byte stripes) with
 *     scalar scramble/merge; scalar accumulate as compile-time twin.
 *   - Seed is fixed at 0 (default secret). 128-bit writes little-endian
 *     lane order: out[0] = low64, out[1] = high64.
 *   - No third-party xxHash source was copied.
 */

#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch98_marker[] = "libcgj-batch98";

/* Official XXH3 default secret (192 bytes) — public constants. */
static const unsigned char s_b98_secret[192] = {
	0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c,
	0xf7, 0x21, 0xad, 0x1c, 0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb,
	0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f, 0xcb, 0x79, 0xe6, 0x4e,
	0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
	0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6,
	0x81, 0x3a, 0x26, 0x4c, 0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb,
	0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3, 0x71, 0x64, 0x48, 0x97,
	0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
	0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7,
	0xc7, 0x0b, 0x4f, 0x1d, 0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31,
	0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64, 0xea, 0xc5, 0xac, 0x83,
	0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
	0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26,
	0x29, 0xd4, 0x68, 0x9e, 0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc,
	0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce, 0x45, 0xcb, 0x3a, 0x8f,
	0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};

#define B98_PRIME32_1 0x9E3779B1u
#define B98_PRIME32_2 0x85EBCA77u
#define B98_PRIME32_3 0xC2B2AE3Du
#define B98_PRIME64_1 0x9E3779B185EBCA87ull
#define B98_PRIME64_2 0xC2B2AE3D27D4EB4Full
#define B98_PRIME64_3 0x165667B19E3779F9ull
#define B98_PRIME64_4 0x85EBCA77C2B2AE63ull
#define B98_PRIME64_5 0x27D4EB2F165667C5ull
#define B98_MIDSIZE_MAX 240u
#define B98_SECRET_SIZE_MIN 136u
#define B98_STRIPE 64u
#define B98_SECRET_CONSUME 8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b98_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static uint64_t
b98_read64le(const unsigned char *p)
{
	return (uint64_t)b98_read32le(p) | ((uint64_t)b98_read32le(p + 4) << 32);
}

static uint64_t
b98_rotl64(uint64_t x, unsigned r)
{
	return (x << r) | (x >> (64u - r));
}

static uint32_t
b98_swap32(uint32_t x)
{
	return ((x & 0x000000ffu) << 24) | ((x & 0x0000ff00u) << 8) |
	       ((x & 0x00ff0000u) >> 8) | ((x & 0xff000000u) >> 24);
}

static uint32_t
b98_rotl32(uint32_t x, unsigned r)
{
	return (x << r) | (x >> (32u - r));
}

static uint64_t
b98_mul128_fold64(uint64_t lhs, uint64_t rhs)
{
	/* Portable 64×64 → 128 fold without __int128 (freestanding). */
	uint64_t lo_lo = (lhs & 0xffffffffull) * (rhs & 0xffffffffull);
	uint64_t hi_lo = (lhs >> 32) * (rhs & 0xffffffffull);
	uint64_t lo_hi = (lhs & 0xffffffffull) * (rhs >> 32);
	uint64_t hi_hi = (lhs >> 32) * (rhs >> 32);
	uint64_t cross = (lo_lo >> 32) + (hi_lo & 0xffffffffull) + lo_hi;
	uint64_t upper = (hi_lo >> 32) + (cross >> 32) + hi_hi;
	uint64_t lower = (cross << 32) | (lo_lo & 0xffffffffull);

	return upper ^ lower;
}

static uint64_t
b98_avalanche(uint64_t h)
{
	h ^= h >> 37;
	h *= 0x165667919E3779F9ull;
	h ^= h >> 32;
	return h;
}

static uint64_t
b98_avalanche_xxh64(uint64_t h)
{
	h ^= h >> 33;
	h *= B98_PRIME64_2;
	h ^= h >> 29;
	h *= B98_PRIME64_3;
	h ^= h >> 32;
	return h;
}

static uint64_t
b98_rrmxmx(uint64_t h, uint64_t len)
{
	h ^= b98_rotl64(h, 49) ^ b98_rotl64(h, 24);
	h *= 0x9FB21C651E98DF25ull;
	h ^= (h >> 35) + len;
	h *= 0x9FB21C651E98DF25ull;
	h ^= h >> 28;
	return h;
}

static uint64_t
b98_mix16(const unsigned char *pIn, const unsigned char *pSec, uint64_t seed)
{
	uint64_t in_lo = b98_read64le(pIn);
	uint64_t in_hi = b98_read64le(pIn + 8);

	return b98_mul128_fold64(in_lo ^ (b98_read64le(pSec) + seed),
	                         in_hi ^ (b98_read64le(pSec + 8) - seed));
}

/* ---- 64-bit short / mid paths (scalar) --------------------------------- */

static uint64_t
b98_len_1to3_64(const unsigned char *p, size_t len, uint64_t seed,
                const unsigned char *secret)
{
	unsigned char c1 = p[0];
	unsigned char c2 = p[len >> 1];
	unsigned char c3 = p[len - 1u];
	uint32_t combined = ((uint32_t)c1 << 16) | ((uint32_t)c2 << 24) |
	                    (uint32_t)c3 | ((uint32_t)len << 8);
	uint64_t bitflip =
	    ((uint64_t)b98_read32le(secret) ^ (uint64_t)b98_read32le(secret + 4)) +
	    seed;

	return b98_avalanche((uint64_t)combined ^ bitflip);
}

static uint64_t
b98_len_4to8_64(const unsigned char *p, size_t len, uint64_t seed,
                const unsigned char *secret)
{
	uint32_t in1 = b98_read32le(p);
	uint32_t in2 = b98_read32le(p + len - 4u);
	uint64_t bitflip =
	    (b98_read64le(secret + 8) ^ b98_read64le(secret + 16)) - seed;
	uint64_t input_64 = (uint64_t)in1 + ((uint64_t)in2 << 32);
	uint64_t keyed = input_64 ^ bitflip;

	return b98_rrmxmx(keyed, len);
}

static uint64_t
b98_len_9to16_64(const unsigned char *p, size_t len, uint64_t seed,
                 const unsigned char *secret)
{
	uint64_t bitflip1 =
	    (b98_read64le(secret + 24) ^ b98_read64le(secret + 32)) + seed;
	uint64_t bitflip2 =
	    (b98_read64le(secret + 40) ^ b98_read64le(secret + 48)) - seed;
	uint64_t in_lo = b98_read64le(p) ^ bitflip1;
	uint64_t in_hi = b98_read64le(p + len - 8u) ^ bitflip2;
	uint64_t acc = (uint64_t)len + b98_rotl64(in_lo, 32) + in_hi +
	               b98_mul128_fold64(in_lo, in_hi);

	return b98_avalanche(acc);
}

static uint64_t
b98_len_0to16_64(const unsigned char *p, size_t len, uint64_t seed,
                 const unsigned char *secret)
{
	if (len > 8u) {
		return b98_len_9to16_64(p, len, seed, secret);
	}
	if (len >= 4u) {
		return b98_len_4to8_64(p, len, seed, secret);
	}
	if (len > 0u) {
		return b98_len_1to3_64(p, len, seed, secret);
	}
	return b98_avalanche_xxh64(
	    seed ^ (b98_read64le(secret + 56) ^ b98_read64le(secret + 64)));
}

static uint64_t
b98_len_17to128_64(const unsigned char *p, size_t len, uint64_t seed,
                   const unsigned char *secret)
{
	uint64_t acc = (uint64_t)len * B98_PRIME64_1;

	if (len > 32u) {
		if (len > 64u) {
			if (len > 96u) {
				acc += b98_mix16(p + 48, secret + 96, seed);
				acc += b98_mix16(p + len - 64u, secret + 112, seed);
			}
			acc += b98_mix16(p + 32, secret + 64, seed);
			acc += b98_mix16(p + len - 48u, secret + 80, seed);
		}
		acc += b98_mix16(p + 16, secret + 32, seed);
		acc += b98_mix16(p + len - 32u, secret + 48, seed);
	}
	acc += b98_mix16(p, secret, seed);
	acc += b98_mix16(p + len - 16u, secret + 16, seed);
	return b98_avalanche(acc);
}

static uint64_t
b98_len_129to240_64(const unsigned char *p, size_t len, uint64_t seed,
                    const unsigned char *secret)
{
	uint64_t acc = (uint64_t)len * B98_PRIME64_1;
	size_t nbRounds = len / 16u;
	size_t i;

	for (i = 0; i < 8u; i++) {
		acc += b98_mix16(p + 16u * i, secret + 16u * i, seed);
	}
	acc = b98_avalanche(acc);
	for (i = 8; i < nbRounds; i++) {
		acc += b98_mix16(p + 16u * i, secret + 16u * (i - 8u) + 3u, seed);
	}
	acc += b98_mix16(p + len - 16u, secret + B98_SECRET_SIZE_MIN - 17u, seed);
	return b98_avalanche(acc);
}

/* ---- 128-bit short / mid paths (scalar) -------------------------------- */

static void
b98_len_1to3_128(const unsigned char *p, size_t len, uint64_t seed,
                 const unsigned char *secret, uint64_t out[2])
{
	unsigned char c1 = p[0];
	unsigned char c2 = p[len >> 1];
	unsigned char c3 = p[len - 1u];
	uint32_t combined =
	    ((uint32_t)c1 << 16) | ((uint32_t)c2 << 24) | (uint32_t)c3 |
	    ((uint32_t)len << 8);
	uint32_t combinedh = b98_rotl32(b98_swap32(combined), 13);
	uint64_t bitflipl =
	    ((uint64_t)b98_read32le(secret) ^ (uint64_t)b98_read32le(secret + 4)) +
	    seed;
	uint64_t bitfliph =
	    ((uint64_t)b98_read32le(secret + 8) ^
	     (uint64_t)b98_read32le(secret + 12)) -
	    seed;

	out[0] = b98_avalanche((uint64_t)combined ^ bitflipl);
	out[1] = b98_avalanche((uint64_t)combinedh ^ bitfliph);
}

static void
b98_len_4to8_128(const unsigned char *p, size_t len, uint64_t seed,
                 const unsigned char *secret, uint64_t out[2])
{
	uint32_t in1 = b98_read32le(p);
	uint32_t in2 = b98_read32le(p + len - 4u);
	uint64_t input_64 = (uint64_t)in1 + ((uint64_t)in2 << 32);
	uint64_t bitflip =
	    (b98_read64le(secret + 16) ^ b98_read64le(secret + 24)) + seed;
	uint64_t keyed = input_64 ^ bitflip;
	uint64_t m128_lo, m128_hi;
	/* 64×64 → 128 multiply of keyed * PRIME64_1, portable. */
	{
		uint64_t lhs = keyed;
		uint64_t rhs = B98_PRIME64_1;
		uint64_t lo_lo = (lhs & 0xffffffffull) * (rhs & 0xffffffffull);
		uint64_t hi_lo = (lhs >> 32) * (rhs & 0xffffffffull);
		uint64_t lo_hi = (lhs & 0xffffffffull) * (rhs >> 32);
		uint64_t hi_hi = (lhs >> 32) * (rhs >> 32);
		uint64_t cross = (lo_lo >> 32) + (hi_lo & 0xffffffffull) + lo_hi;

		m128_lo = (cross << 32) | (lo_lo & 0xffffffffull);
		m128_hi = (hi_lo >> 32) + (cross >> 32) + hi_hi;
	}
	m128_hi += (uint64_t)len * ((uint32_t)B98_PRIME32_1);
	out[0] = b98_avalanche(m128_lo);
	out[1] = b98_avalanche(m128_hi);
}

static void
b98_len_9to16_128(const unsigned char *p, size_t len, uint64_t seed,
                  const unsigned char *secret, uint64_t out[2])
{
	uint64_t bitflipl =
	    (b98_read64le(secret + 32) ^ b98_read64le(secret + 40)) - seed;
	uint64_t bitfliph =
	    (b98_read64le(secret + 48) ^ b98_read64le(secret + 56)) + seed;
	uint64_t input_lo = b98_read64le(p);
	uint64_t input_hi = b98_read64le(p + len - 8u);
	uint64_t m128_lo, m128_hi;
	uint64_t lo, hi;

	lo = input_lo ^ bitflipl;
	hi = input_hi ^ bitfliph;
	/* lo * hi → 128 */
	{
		uint64_t lo_lo = (lo & 0xffffffffull) * (hi & 0xffffffffull);
		uint64_t hi_lo = (lo >> 32) * (hi & 0xffffffffull);
		uint64_t lo_hi = (lo & 0xffffffffull) * (hi >> 32);
		uint64_t hi_hi = (lo >> 32) * (hi >> 32);
		uint64_t cross = (lo_lo >> 32) + (hi_lo & 0xffffffffull) + lo_hi;

		m128_lo = (cross << 32) | (lo_lo & 0xffffffffull);
		m128_hi = (hi_lo >> 32) + (cross >> 32) + hi_hi;
	}
	m128_lo += (uint64_t)len * B98_PRIME64_1 + b98_rotl64(input_hi, 32);
	m128_hi += b98_rotl64(input_lo, 32) + input_hi;
	out[0] = b98_avalanche(m128_lo);
	out[1] = b98_avalanche(m128_hi);
}

static void
b98_len_0to16_128(const unsigned char *p, size_t len, uint64_t seed,
                  const unsigned char *secret, uint64_t out[2])
{
	if (len > 8u) {
		b98_len_9to16_128(p, len, seed, secret, out);
		return;
	}
	if (len >= 4u) {
		b98_len_4to8_128(p, len, seed, secret, out);
		return;
	}
	if (len > 0u) {
		b98_len_1to3_128(p, len, seed, secret, out);
		return;
	}
	{
		uint64_t bitflipl =
		    b98_read64le(secret + 64) ^ b98_read64le(secret + 72);
		uint64_t bitfliph =
		    b98_read64le(secret + 80) ^ b98_read64le(secret + 88);

		out[0] = b98_avalanche_xxh64(seed ^ bitflipl);
		out[1] = b98_avalanche_xxh64(seed ^ bitfliph);
	}
}

static void
b98_mix32_128(uint64_t *acc0, uint64_t *acc1, const unsigned char *pIn,
              const unsigned char *pSec, uint64_t seed)
{
	/* One 32-byte mix for mid-size 128-bit path. */
	uint64_t lo = b98_mix16(pIn, pSec, seed);
	uint64_t hi = b98_mix16(pIn + 16, pSec + 16, seed);

	*acc0 += lo;
	*acc1 += hi;
}

static void
b98_len_17to128_128(const unsigned char *p, size_t len, uint64_t seed,
                    const unsigned char *secret, uint64_t out[2])
{
	uint64_t acc0 = (uint64_t)len * B98_PRIME64_1;
	uint64_t acc1 = 0;

	if (len > 32u) {
		if (len > 64u) {
			if (len > 96u) {
				b98_mix32_128(&acc0, &acc1, p + 48, secret + 96, seed);
				b98_mix32_128(&acc0, &acc1, p + len - 64u, secret + 128,
				              seed);
			}
			b98_mix32_128(&acc0, &acc1, p + 32, secret + 64, seed);
			b98_mix32_128(&acc0, &acc1, p + len - 48u, secret + 96, seed);
		}
		b98_mix32_128(&acc0, &acc1, p + 16, secret + 32, seed);
		b98_mix32_128(&acc0, &acc1, p + len - 32u, secret + 64, seed);
	}
	b98_mix32_128(&acc0, &acc1, p, secret, seed);
	b98_mix32_128(&acc0, &acc1, p + len - 16u, secret + 32, seed);
	/* Mid-size 128 finalize: fold pair. */
	{
		uint64_t h128_lo = acc0 + acc1;
		uint64_t h128_hi =
		    (acc0 * B98_PRIME64_1) + (acc1 * B98_PRIME64_4) +
		    ((uint64_t)len - seed) * B98_PRIME64_2;

		out[0] = b98_avalanche(h128_lo);
		out[1] = b98_avalanche(h128_hi);
	}
}

static void
b98_len_129to240_128(const unsigned char *p, size_t len, uint64_t seed,
                     const unsigned char *secret, uint64_t out[2])
{
	uint64_t acc0 = (uint64_t)len * B98_PRIME64_1;
	uint64_t acc1 = 0;
	size_t nbRounds = len / 32u;
	size_t i;

	for (i = 0; i < 4u; i++) {
		b98_mix32_128(&acc0, &acc1, p + 32u * i, secret + 32u * i, seed);
	}
	acc0 = b98_avalanche(acc0);
	acc1 = b98_avalanche(acc1);
	for (i = 4; i < nbRounds; i++) {
		b98_mix32_128(&acc0, &acc1, p + 32u * i,
		              secret + 3u + 32u * (i - 4u), seed);
	}
	/* last 32 bytes */
	b98_mix32_128(&acc0, &acc1, p + len - 32u,
	              secret + B98_SECRET_SIZE_MIN - 17u - 16u, seed);
	{
		uint64_t h128_lo = acc0 + acc1;
		uint64_t h128_hi =
		    (acc0 * B98_PRIME64_1) + (acc1 * B98_PRIME64_4) +
		    ((uint64_t)len - seed) * B98_PRIME64_2;

		out[0] = b98_avalanche(h128_lo);
		out[1] = b98_avalanche(h128_hi);
	}
}

/* ---- long path: SSE2 + scalar accumulate ------------------------------- */

/*
 * Scalar accumulate_512: for each of 8 lanes
 *   data_val = input[i]
 *   data_key = data_val ^ secret[i]
 *   acc[i ^ 1] += data_val
 *   acc[i]     += (uint32_t)data_key * (data_key >> 32)
 */
static void
b98_accumulate_512_scalar(uint64_t *acc, const unsigned char *pIn,
                          const unsigned char *pSec)
{
	size_t i;

	for (i = 0; i < 8u; i++) {
		uint64_t data_val = b98_read64le(pIn + 8u * i);
		uint64_t data_key = data_val ^ b98_read64le(pSec + 8u * i);

		acc[i ^ 1u] += data_val;
		acc[i] += (data_key & 0xffffffffull) * (data_key >> 32);
	}
}

/*
 * SSE2 accumulate over one 64-byte stripe (4 × 16-byte chunks).
 * Each XMM holds two 64-bit lanes; product uses _mm_mul_epu32 on the
 * even 32-bit sub-lanes of (data^secret) × (data^secret >> 32).
 * Lane swap via _mm_shuffle_epi32 implements acc[i^1] += data_val.
 */
static void
b98_accumulate_512_sse2(uint64_t *acc, const unsigned char *pIn,
                        const unsigned char *pSec)
{
	size_t i;

	for (i = 0; i < 4u; i++) {
		__m128i vdata =
		    _mm_loadu_si128((const __m128i *)(const void *)(pIn + 16u * i));
		__m128i vkey =
		    _mm_loadu_si128((const __m128i *)(const void *)(pSec + 16u * i));
		__m128i vacc =
		    _mm_loadu_si128((const __m128i *)(const void *)(acc + 2u * i));
		__m128i vdk = _mm_xor_si128(vdata, vkey);
		/* 32×32→64 on even dword lanes: lo * hi of each u64 */
		__m128i vprod =
		    _mm_mul_epu32(vdk, _mm_srli_epi64(vdk, 32));
		/* swap the two u64 lanes for acc[i^1] += data */
		__m128i vswap = _mm_shuffle_epi32(vdata, 0x4e); /* (1,0,3,2) as dwords */

		vacc = _mm_add_epi64(vacc, vswap);
		vacc = _mm_add_epi64(vacc, vprod);
		_mm_storeu_si128((__m128i *)(void *)(acc + 2u * i), vacc);
	}
}

static void
b98_scramble_acc(uint64_t *acc, const unsigned char *pSec)
{
	size_t i;

	for (i = 0; i < 8u; i++) {
		uint64_t key64 = b98_read64le(pSec + 8u * i);
		uint64_t acc64 = acc[i];

		acc64 ^= acc64 >> 47;
		acc64 ^= key64;
		acc64 *= (uint64_t)B98_PRIME32_1;
		acc[i] = acc64;
	}
}

static uint64_t
b98_merge_accs(const uint64_t *acc, const unsigned char *secret, uint64_t start)
{
	uint64_t result = start;
	size_t i;

	for (i = 0; i < 4u; i++) {
		result += b98_mul128_fold64(
		    acc[2u * i] ^ b98_read64le(secret + 16u * i),
		    acc[2u * i + 1u] ^ b98_read64le(secret + 16u * i + 8u));
	}
	return b98_avalanche(result);
}

static void
b98_init_acc(uint64_t acc[8])
{
	acc[0] = (uint64_t)B98_PRIME32_3;
	acc[1] = B98_PRIME64_1;
	acc[2] = B98_PRIME64_2;
	acc[3] = B98_PRIME64_3;
	acc[4] = B98_PRIME64_4;
	acc[5] = (uint64_t)B98_PRIME32_2;
	acc[6] = B98_PRIME64_5;
	acc[7] = (uint64_t)B98_PRIME32_1;
}

/*
 * Long hash: process full blocks of stripes with scramble between blocks.
 * use_sse2 selects the accumulate path (SSE2 preferred for public API).
 */
static void
b98_hashLong(const unsigned char *pIn, size_t len, const unsigned char *secret,
             size_t secretSize, int use_sse2, uint64_t *pOut64, uint64_t out128[2])
{
	uint64_t acc[8];
	size_t nb_stripes_per_block;
	size_t nb_blocks;
	size_t n;
	size_t nbStripes;
	const size_t stripe = B98_STRIPE;
	const size_t secretConsume = B98_SECRET_CONSUME;

	b98_init_acc(acc);

	nb_stripes_per_block = (secretSize - stripe) / secretConsume;
	nb_blocks = (len - 1u) / (stripe * nb_stripes_per_block);

	for (n = 0; n < nb_blocks; n++) {
		size_t s;

		for (s = 0; s < nb_stripes_per_block; s++) {
			const unsigned char *pStripe =
			    pIn + n * nb_stripes_per_block * stripe + s * stripe;
			const unsigned char *pSec = secret + s * secretConsume;

			if (use_sse2) {
				b98_accumulate_512_sse2(acc, pStripe, pSec);
			} else {
				b98_accumulate_512_scalar(acc, pStripe, pSec);
			}
		}
		b98_scramble_acc(acc, secret + secretSize - stripe);
	}

	nbStripes =
	    ((len - 1u) - (stripe * nb_stripes_per_block * nb_blocks)) / stripe;
	for (n = 0; n < nbStripes; n++) {
		const unsigned char *pStripe =
		    pIn + nb_blocks * nb_stripes_per_block * stripe + n * stripe;
		const unsigned char *pSec = secret + n * secretConsume;

		if (use_sse2) {
			b98_accumulate_512_sse2(acc, pStripe, pSec);
		} else {
			b98_accumulate_512_scalar(acc, pStripe, pSec);
		}
	}
	/* last stripe always aligned to end of input */
	if (use_sse2) {
		b98_accumulate_512_sse2(acc, pIn + len - stripe,
		                        secret + secretSize - stripe - 7u);
	} else {
		b98_accumulate_512_scalar(acc, pIn + len - stripe,
		                          secret + secretSize - stripe - 7u);
	}

	if (pOut64 != NULL) {
		*pOut64 = b98_merge_accs(acc, secret + 11u,
		                         (uint64_t)len * B98_PRIME64_1);
	}
	if (out128 != NULL) {
		out128[0] = b98_merge_accs(acc, secret + 11u,
		                           (uint64_t)len * B98_PRIME64_1);
		out128[1] = b98_merge_accs(
		    acc, secret + secretSize - stripe - 11u,
		    ~((uint64_t)len * B98_PRIME64_2));
	}
}

/* ---- public API -------------------------------------------------------- */

/*
 * gj_xxh3_64 — one-shot 64-bit hash (seed 0, default secret).
 * Short/mid: scalar. Long: SSE2 accumulate.
 * On well-formed inputs should match batch41 XXH3_64bits.
 */
uint64_t
gj_xxh3_64(const void *data, size_t len)
{
	const unsigned char *p = (const unsigned char *)data;

	if (p == NULL && len != 0u) {
		return 0ull;
	}
	if (len <= 16u) {
		return b98_len_0to16_64(p, len, 0ull, s_b98_secret);
	}
	if (len <= 128u) {
		return b98_len_17to128_64(p, len, 0ull, s_b98_secret);
	}
	if (len <= B98_MIDSIZE_MAX) {
		return b98_len_129to240_64(p, len, 0ull, s_b98_secret);
	}
	{
		uint64_t h;

		b98_hashLong(p, len, s_b98_secret, sizeof(s_b98_secret), 1, &h,
		             NULL);
		return h;
	}
}

/*
 * gj_xxh3_128 — one-shot 128-bit hash (seed 0, default secret).
 * Writes out[0] = low64, out[1] = high64. No-op if out is NULL.
 * Long path uses SSE2 accumulate; short/mid are scalar.
 */
void
gj_xxh3_128(const void *data, size_t len, uint64_t out[2])
{
	const unsigned char *p = (const unsigned char *)data;

	if (out == NULL) {
		return;
	}
	if (p == NULL && len != 0u) {
		out[0] = 0ull;
		out[1] = 0ull;
		return;
	}
	if (len <= 16u) {
		b98_len_0to16_128(p, len, 0ull, s_b98_secret, out);
		return;
	}
	if (len <= 128u) {
		b98_len_17to128_128(p, len, 0ull, s_b98_secret, out);
		return;
	}
	if (len <= B98_MIDSIZE_MAX) {
		b98_len_129to240_128(p, len, 0ull, s_b98_secret, out);
		return;
	}
	b98_hashLong(p, len, s_b98_secret, sizeof(s_b98_secret), 1, NULL, out);
}

uint64_t __gj_xxh3_64(const void *data, size_t len)
    __attribute__((alias("gj_xxh3_64")));
void __gj_xxh3_128(const void *data, size_t len, uint64_t out[2])
    __attribute__((alias("gj_xxh3_128")));
