/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch184: Hamming distance over byte buffers.
 * Clean-room freestanding pure C — SWAR scalar paths; optional SSE2
 * bulk path for XOR+popcount. Builds under userspace -msse2. No
 * third-party source was copied. Does not rely on __builtin_popcount*
 * (those expand to libgcc/libcalls under -ffreestanding without -mpopcnt).
 *
 * Surface (unique symbols):
 *   int gj_hamming_distance(const void *a, const void *b, size_t n);
 *     — number of differing bits across n bytes of a and b
 *       (popcount of a[i] XOR b[i] for i in [0, n)). Returns 0 if
 *       a or b is NULL or n == 0. SSE2 path when n >= 16 on x86/x86-64.
 *   __gj_hamming_distance  (alias)
 *   __libcgj_batch184_marker = "libcgj-batch184"
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

const char __libcgj_batch184_marker[] = "libcgj-batch184";

/* ---- SWAR scalar popcount ---------------------------------------------- */

/*
 * Hamming weight via parallel bit sums (Hacker's Delight / SWAR).
 * No multiplies; safe for freestanding -fno-builtin -msse2.
 */
static unsigned
b184_popcount32(uint32_t x)
{
	x = x - ((x >> 1) & 0x55555555u);
	x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
	x = (x + (x >> 4)) & 0x0f0f0f0fu;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return (unsigned)(x & 0x3fu);
}

static unsigned
b184_popcount64(uint64_t x)
{
	x = x - ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
	x = x + (x >> 8);
	x = x + (x >> 16);
	x = x + (x >> 32);
	return (unsigned)(x & 0x7full);
}

/* ---- SSE2 bulk XOR + popcount ------------------------------------------ */

#if defined(__x86_64__) || defined(__i386__)
/*
 * Parallel popcount of 16 bytes via SSE2 SWAR + PSADBW horizontal sum.
 * Each byte is reduced to its bit count (0..8), then two 8-byte sums
 * are formed with _mm_sad_epu8 against zero.
 */
static unsigned
b184_popcnt_m128(__m128i v)
{
	const __m128i m1 = _mm_set1_epi8(0x55);
	const __m128i m2 = _mm_set1_epi8(0x33);
	const __m128i m4 = _mm_set1_epi8(0x0f);
	__m128i t;
	__m128i hi;
	unsigned lo_sum;
	unsigned hi_sum;

	t = _mm_sub_epi8(v, _mm_and_si128(_mm_srli_epi16(v, 1), m1));
	t = _mm_add_epi8(_mm_and_si128(t, m2),
	                 _mm_and_si128(_mm_srli_epi16(t, 2), m2));
	t = _mm_add_epi8(_mm_and_si128(t, m4),
	                 _mm_and_si128(_mm_srli_epi16(t, 4), m4));
	t = _mm_sad_epu8(t, _mm_setzero_si128());
	lo_sum = (unsigned)_mm_cvtsi128_si32(t);
	hi = _mm_unpackhi_epi64(t, t);
	hi_sum = (unsigned)_mm_cvtsi128_si32(hi);
	return lo_sum + hi_sum;
}
#endif

/* ---- gj_hamming_distance ----------------------------------------------- */

/*
 * Bit-level Hamming distance between two buffers of length n.
 * Walks optional SSE2 16-byte XOR blocks, then residual uint64/bytes.
 */
int
gj_hamming_distance(const void *a, const void *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;
	unsigned total;

	if (a == NULL || b == NULL || n == 0u) {
		return 0;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	total = 0u;
	i = 0u;

#if defined(__x86_64__) || defined(__i386__)
	if (n >= 16u) {
		size_t nBlk = n & ~(size_t)15u;

		while (i < nBlk) {
			__m128i va = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pa + i));
			__m128i vb = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pb + i));
			__m128i vx = _mm_xor_si128(va, vb);

			total += b184_popcnt_m128(vx);
			i += 16u;
		}
	}
#endif

	/* Residual 8-byte words (unaligned LE loads via byte assemble). */
	while (i + 8u <= n) {
		uint64_t wa;
		uint64_t wb;

		wa = (uint64_t)pa[i]
		    | ((uint64_t)pa[i + 1u] << 8)
		    | ((uint64_t)pa[i + 2u] << 16)
		    | ((uint64_t)pa[i + 3u] << 24)
		    | ((uint64_t)pa[i + 4u] << 32)
		    | ((uint64_t)pa[i + 5u] << 40)
		    | ((uint64_t)pa[i + 6u] << 48)
		    | ((uint64_t)pa[i + 7u] << 56);
		wb = (uint64_t)pb[i]
		    | ((uint64_t)pb[i + 1u] << 8)
		    | ((uint64_t)pb[i + 2u] << 16)
		    | ((uint64_t)pb[i + 3u] << 24)
		    | ((uint64_t)pb[i + 4u] << 32)
		    | ((uint64_t)pb[i + 5u] << 40)
		    | ((uint64_t)pb[i + 6u] << 48)
		    | ((uint64_t)pb[i + 7u] << 56);
		total += b184_popcount64(wa ^ wb);
		i += 8u;
	}

	while (i < n) {
		total += b184_popcount32((uint32_t)(pa[i] ^ pb[i]));
		i++;
	}

	return (int)total;
}

int __gj_hamming_distance(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_hamming_distance")));
