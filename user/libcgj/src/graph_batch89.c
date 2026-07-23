/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch89: SSE2-accelerated string/memory helpers.
 * Clean-room freestanding C with SSE2 (-msse2). Userspace only.
 *
 * Existing surface NOT redefined (avoid multi-def with string.c):
 *   strlen / strchr / memcmp → string.c (scalar)
 *
 * This TU adds only unique gj_*_sse2 symbols:
 *   size_t gj_strlen_sse2(const char *s);
 *   char  *gj_strchr_sse2(const char *s, int c);
 *   void  *gj_memchr_sse2(const void *s, int c, size_t n);
 *   int    gj_memcmp_sse2(const void *a, const void *b, size_t n);
 *   __gj_strlen_sse2 / __gj_strchr_sse2 / __gj_memchr_sse2 / __gj_memcmp_sse2
 *   __libcgj_batch89_marker = "libcgj-batch89"
 *
 * Correct for all inputs; handles unaligned pointers. Main loops use
 * 16-byte-aligned loads so page-boundary overreads are avoided.
 * No third-party SIMD string source was copied.
 *
 * Soft deepen: bounded memchr (no page overread past n), underscored
 * aliases, NULL short-circuits.
 */

#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch89_marker[] = "libcgj-batch89";

/* --------------------------------------------------------------------------
 * gj_strlen_sse2 — NUL-terminated length via SSE2 pcmpeqb + movemask
 * -------------------------------------------------------------------------- */

size_t
gj_strlen_sse2(const char *s)
{
	const char *pSz;
	const char *pAligned;
	__m128i vZero;
	__m128i vChunk;
	unsigned int uMask;
	uintptr_t uAddr;
	size_t cbHead;

	if (s == NULL) {
		return 0;
	}

	pSz = s;
	uAddr = (uintptr_t)(const void *)pSz;

	/* Byte-scan until 16-byte alignment (safe page-edge head). */
	cbHead = uAddr & 15u;
	if (cbHead != 0) {
		size_t cbToAlign = 16u - cbHead;
		size_t i;

		for (i = 0; i < cbToAlign; i++) {
			if (pSz[i] == '\0') {
				return i;
			}
		}
		pSz += cbToAlign;
	}

	vZero = _mm_setzero_si128();
	pAligned = pSz;

	for (;;) {
		/* Aligned load: 16B block cannot cross a 4KiB page boundary. */
		vChunk = _mm_load_si128((const __m128i *)(const void *)pAligned);
		uMask = (unsigned int)_mm_movemask_epi8(
		    _mm_cmpeq_epi8(vChunk, vZero));
		if (uMask != 0u) {
			/* Lowest set bit = first NUL offset in this block. */
			return (size_t)(pAligned - s) +
			       (size_t)__builtin_ctz(uMask);
		}
		pAligned += 16;
	}
}

/* --------------------------------------------------------------------------
 * gj_strchr_sse2 — first occurrence of c (including NUL) via SSE2
 * -------------------------------------------------------------------------- */

char *
gj_strchr_sse2(const char *s, int c)
{
	const char *pSz;
	const char *pAligned;
	__m128i vNeedle;
	__m128i vZero;
	__m128i vChunk;
	__m128i vEqC;
	__m128i vEqZ;
	unsigned int uMaskC;
	unsigned int uMaskZ;
	unsigned int uMask;
	uintptr_t uAddr;
	unsigned char chNeedle;
	size_t cbHead;

	if (s == NULL) {
		return NULL;
	}

	chNeedle = (unsigned char)c;
	pSz = s;
	uAddr = (uintptr_t)(const void *)pSz;

	/* Head to alignment. */
	cbHead = uAddr & 15u;
	if (cbHead != 0) {
		size_t cbToAlign = 16u - cbHead;
		size_t i;

		for (i = 0; i < cbToAlign; i++) {
			unsigned char ch = (unsigned char)pSz[i];

			if (ch == chNeedle) {
				return (char *)(uintptr_t)(pSz + i);
			}
			if (ch == '\0') {
				return NULL;
			}
		}
		pSz += cbToAlign;
	}

	vNeedle = _mm_set1_epi8((char)chNeedle);
	vZero = _mm_setzero_si128();
	pAligned = pSz;

	for (;;) {
		vChunk = _mm_load_si128((const __m128i *)(const void *)pAligned);
		vEqC = _mm_cmpeq_epi8(vChunk, vNeedle);
		vEqZ = _mm_cmpeq_epi8(vChunk, vZero);
		uMaskC = (unsigned int)_mm_movemask_epi8(vEqC);
		uMaskZ = (unsigned int)_mm_movemask_epi8(vEqZ);
		uMask = uMaskC | uMaskZ;
		if (uMask != 0u) {
			unsigned int uBit = (unsigned int)__builtin_ctz(uMask);
			unsigned int uHit = 1u << uBit;

			/* Match wins only if it appears at or before first NUL. */
			if ((uMaskC & uHit) != 0u) {
				return (char *)(uintptr_t)(pAligned + uBit);
			}
			return NULL;
		}
		pAligned += 16;
	}
}

/* --------------------------------------------------------------------------
 * gj_memchr_sse2 — first byte == c within s[0..n); soft deepen bounded scan
 * -------------------------------------------------------------------------- */

void *
gj_memchr_sse2(const void *s, int c, size_t n)
{
	const unsigned char *p;
	const unsigned char *pEnd;
	const unsigned char *pAligned;
	unsigned char chNeedle;
	__m128i vNeedle;
	__m128i vChunk;
	unsigned int uMask;
	uintptr_t uAddr;
	size_t cbHead;
	size_t cbLeft;

	if (s == NULL || n == 0u) {
		return NULL;
	}

	p = (const unsigned char *)s;
	pEnd = p + n;
	chNeedle = (unsigned char)c;
	uAddr = (uintptr_t)(const void *)p;

	/* Head to 16-byte alignment without reading past n. */
	cbHead = uAddr & 15u;
	if (cbHead != 0u) {
		size_t cbToAlign = 16u - cbHead;
		size_t i;

		if (cbToAlign > n) {
			cbToAlign = n;
		}
		for (i = 0; i < cbToAlign; i++) {
			if (p[i] == chNeedle) {
				return (void *)(uintptr_t)(p + i);
			}
		}
		p += cbToAlign;
	}

	vNeedle = _mm_set1_epi8((char)chNeedle);
	pAligned = p;
	cbLeft = (size_t)(pEnd - pAligned);

	while (cbLeft >= 16u) {
		vChunk = _mm_load_si128((const __m128i *)(const void *)pAligned);
		uMask = (unsigned int)_mm_movemask_epi8(
		    _mm_cmpeq_epi8(vChunk, vNeedle));
		if (uMask != 0u) {
			return (void *)(uintptr_t)(pAligned +
			                           (size_t)__builtin_ctz(uMask));
		}
		pAligned += 16;
		cbLeft -= 16u;
	}

	while (pAligned < pEnd) {
		if (*pAligned == chNeedle) {
			return (void *)(uintptr_t)pAligned;
		}
		pAligned++;
	}
	return NULL;
}

/* --------------------------------------------------------------------------
 * gj_memcmp_sse2 — lexicographic byte compare, SSE2 bulk path
 * -------------------------------------------------------------------------- */

int
gj_memcmp_sse2(const void *a, const void *b, size_t n)
{
	const unsigned char *pA;
	const unsigned char *pB;
	size_t cbLeft;
	size_t i;

	if (a == NULL || b == NULL) {
		if (a == b) {
			return 0;
		}
		return (a == NULL) ? -1 : 1;
	}
	if (n == 0) {
		return 0;
	}

	pA = (const unsigned char *)a;
	pB = (const unsigned char *)b;
	cbLeft = n;

	/*
	 * Unaligned 16B loads are fine here: both sides are bounded by n,
	 * so we only touch [base, base+n). Full blocks first; scalar tail.
	 */
	while (cbLeft >= 16u) {
		__m128i vA = _mm_loadu_si128((const __m128i *)(const void *)pA);
		__m128i vB = _mm_loadu_si128((const __m128i *)(const void *)pB);
		__m128i vEq = _mm_cmpeq_epi8(vA, vB);
		unsigned int uMask =
		    (unsigned int)_mm_movemask_epi8(vEq);

		if (uMask != 0xffffu) {
			unsigned int uDiff =
			    (unsigned int)__builtin_ctz(~uMask & 0xffffu);

			return (int)pA[uDiff] - (int)pB[uDiff];
		}
		pA += 16;
		pB += 16;
		cbLeft -= 16u;
	}

	for (i = 0; i < cbLeft; i++) {
		if (pA[i] != pB[i]) {
			return (int)pA[i] - (int)pB[i];
		}
	}
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

size_t __gj_strlen_sse2(const char *s)
    __attribute__((alias("gj_strlen_sse2")));
char *__gj_strchr_sse2(const char *s, int c)
    __attribute__((alias("gj_strchr_sse2")));
void *__gj_memchr_sse2(const void *s, int c, size_t n)
    __attribute__((alias("gj_memchr_sse2")));
int __gj_memcmp_sse2(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_memcmp_sse2")));

