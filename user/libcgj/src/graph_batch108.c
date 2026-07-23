/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch108: IEEE CRC-32 with PCLMULQDQ folding.
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   crc32 / crc32_z / __crc32_z                     (graph_batch39.c)
 *   crc32c / crc32c_hw / crc32c_auto                (batch39 / batch87)
 *   crc64 / crc64_ecma / crc64_update / crc64_xz    (batch41 / batch67)
 *   gj_cpu_has_pclmul / ghash_pclmul                (graph_batch85.c)
 *
 * This TU adds only unique symbols:
 *   uint32_t crc32_pclmul(uint32_t crc, const void *buf, size_t len);
 *     — IEEE / zlib-shaped CRC-32 (poly 0xEDB88320 reflected).
 *       PCLMUL 16/64-byte folding when CPUID.1:ECX[1] is set and len >= 16;
 *       otherwise table path matching batch39 crc32 / crc32_z chaining.
 *       Init crc=0 for one-shot; Check("123456789") == 0xcbf43926.
 *   __crc32_pclmul (alias)
 *   __libcgj_batch108_marker = "libcgj-batch108"
 *
 * Clean-room from the public Intel whitepaper algorithm
 * ("Fast CRC Computation for Generic Polynomials Using PCLMULQDQ
 * Instruction") and the standard bit-reflected IEEE CRC-32 fold
 * constants; no third-party source was copied. Compiles freestanding
 * with -msse2 (PCLMUL via target attribute + inline path, not -mpclmul).
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#include <wmmintrin.h>
#endif

const char __libcgj_batch108_marker[] = "libcgj-batch108";

/* ---- local PCLMUL detect (do not redefine gj_cpu_has_pclmul) ----------- */

/* -1 unknown, 0 absent, 1 present */
static int s_b108_pclmul = -1;

static int
b108_has_pclmul(void)
{
#if defined(__x86_64__) || defined(__i386__)
	unsigned uEax;
	unsigned uEbx;
	unsigned uEcx;
	unsigned uEdx;
#endif

	if (s_b108_pclmul >= 0) {
		return s_b108_pclmul;
	}

#if defined(__x86_64__) || defined(__i386__)
	uEax = 0u;
	uEbx = 0u;
	uEcx = 0u;
	uEdx = 0u;
	/* CPUID.1:ECX[1] = PCLMULQDQ */
	__asm__ volatile("cpuid"
	    : "=a"(uEax), "=b"(uEbx), "=c"(uEcx), "=d"(uEdx)
	    : "a"(1u), "c"(0u)
	    : "cc");
	s_b108_pclmul = (int)((uEcx >> 1) & 1u);
	(void)uEax;
	(void)uEbx;
	(void)uEdx;
#else
	s_b108_pclmul = 0;
#endif
	return s_b108_pclmul;
}

/* ---- software table (IEEE; matches batch39 crc32 / crc32_z) ------------ */

static uint32_t s_b108_crc_tab[256];
static int s_b108_tab_ready;

static void
b108_crc_init_tab(void)
{
	/* IEEE poly 0x04C11DB7 reflected → 0xEDB88320 */
	const uint32_t uPoly = 0xedb88320u;
	unsigned i;
	unsigned b;

	if (s_b108_tab_ready) {
		return;
	}
	for (i = 0; i < 256u; i++) {
		uint32_t uC = (uint32_t)i;

		for (b = 0; b < 8u; b++) {
			if ((uC & 1u) != 0u) {
				uC = uPoly ^ (uC >> 1);
			} else {
				uC >>= 1;
			}
		}
		s_b108_crc_tab[i] = uC;
	}
	s_b108_tab_ready = 1;
}

/*
 * Table CRC-32 matching batch39 crc32_z chaining:
 *   out = crc32_z(in, buf, len);  init in=0 for one-shot.
 * NULL buf → 0.
 */
static uint32_t
b108_crc32_sw(uint32_t uCrc, const unsigned char *pBuf, size_t cb)
{
	size_t i;

	if (pBuf == NULL) {
		return 0u;
	}
	b108_crc_init_tab();
	uCrc = ~uCrc;
	for (i = 0; i < cb; i++) {
		uCrc = s_b108_crc_tab[(uCrc ^ pBuf[i]) & 0xffu] ^ (uCrc >> 8);
	}
	return ~uCrc;
}

/* ---- PCLMUL fold body (x86) -------------------------------------------- */

#if defined(__x86_64__) || defined(__i386__)

/*
 * Bit-reflected IEEE CRC-32 fold constants (Intel whitepaper domain):
 *   k1 = x^(32*4+32) / k2 = x^(32*4-32)  — 64-byte parallel fold
 *   k3 = x^(32+32)   / k4 = x^(32-32)+…  — 16-byte fold
 *   k5 = x^32 mod P                      — 128→64 residual fold
 *   poly = P | Barrett mu
 *
 * Preconditions: len >= 16 and (len % 16) == 0.
 * uCrc / return value are in the inverted (~user) domain.
 */
__attribute__((target("pclmul,sse2")))
static uint32_t
b108_crc32_pclmul_body(const unsigned char *pBuf, size_t cb, uint32_t uCrc)
{
	/* 16-byte aligned constant pools (static storage). */
	static const uint64_t s_k1k2[2] = { 0x0154442bd4ULL, 0x01c6e41596ULL };
	static const uint64_t s_k3k4[2] = { 0x01751997d0ULL, 0x00ccaa009eULL };
	static const uint64_t s_k5k0[2] = { 0x0163cd6124ULL, 0x0000000000ULL };
	static const uint64_t s_poly[2] = { 0x01db710641ULL, 0x01f7011641ULL };

	__m128i x0;
	__m128i x1;
	__m128i x2;
	__m128i x3;
	__m128i x4;
	__m128i x5;
	__m128i x6;
	__m128i x7;
	__m128i x8;
	__m128i y5;
	__m128i y6;
	__m128i y7;
	__m128i y8;
	__m128i xMask;

	if (cb >= 64u) {
		x1 = _mm_loadu_si128((const __m128i *)(const void *)(pBuf + 0x00));
		x2 = _mm_loadu_si128((const __m128i *)(const void *)(pBuf + 0x10));
		x3 = _mm_loadu_si128((const __m128i *)(const void *)(pBuf + 0x20));
		x4 = _mm_loadu_si128((const __m128i *)(const void *)(pBuf + 0x30));

		x1 = _mm_xor_si128(x1, _mm_cvtsi32_si128((int)uCrc));
		x0 = _mm_loadu_si128((const __m128i *)(const void *)s_k1k2);

		pBuf += 64;
		cb -= 64u;

		while (cb >= 64u) {
			x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
			x6 = _mm_clmulepi64_si128(x2, x0, 0x00);
			x7 = _mm_clmulepi64_si128(x3, x0, 0x00);
			x8 = _mm_clmulepi64_si128(x4, x0, 0x00);

			x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
			x2 = _mm_clmulepi64_si128(x2, x0, 0x11);
			x3 = _mm_clmulepi64_si128(x3, x0, 0x11);
			x4 = _mm_clmulepi64_si128(x4, x0, 0x11);

			y5 = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pBuf + 0x00));
			y6 = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pBuf + 0x10));
			y7 = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pBuf + 0x20));
			y8 = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pBuf + 0x30));

			x1 = _mm_xor_si128(x1, x5);
			x2 = _mm_xor_si128(x2, x6);
			x3 = _mm_xor_si128(x3, x7);
			x4 = _mm_xor_si128(x4, x8);

			x1 = _mm_xor_si128(x1, y5);
			x2 = _mm_xor_si128(x2, y6);
			x3 = _mm_xor_si128(x3, y7);
			x4 = _mm_xor_si128(x4, y8);

			pBuf += 64;
			cb -= 64u;
		}

		/* Collapse four 128-bit lanes → one 128-bit state (k3/k4). */
		x0 = _mm_loadu_si128((const __m128i *)(const void *)s_k3k4);

		x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
		x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
		x1 = _mm_xor_si128(x1, x2);
		x1 = _mm_xor_si128(x1, x5);

		x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
		x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
		x1 = _mm_xor_si128(x1, x3);
		x1 = _mm_xor_si128(x1, x5);

		x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
		x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
		x1 = _mm_xor_si128(x1, x4);
		x1 = _mm_xor_si128(x1, x5);
	} else {
		/* 16 <= cb < 64: single 128-bit stream fold. */
		x1 = _mm_loadu_si128((const __m128i *)(const void *)pBuf);
		x1 = _mm_xor_si128(x1, _mm_cvtsi32_si128((int)uCrc));
		pBuf += 16;
		cb -= 16u;
		x0 = _mm_loadu_si128((const __m128i *)(const void *)s_k3k4);
	}

	while (cb >= 16u) {
		x2 = _mm_loadu_si128((const __m128i *)(const void *)pBuf);
		x5 = _mm_clmulepi64_si128(x1, x0, 0x00);
		x1 = _mm_clmulepi64_si128(x1, x0, 0x11);
		x1 = _mm_xor_si128(x1, x2);
		x1 = _mm_xor_si128(x1, x5);
		pBuf += 16;
		cb -= 16u;
	}

	/* Fold 128 → 96/64 bits. */
	x2 = _mm_clmulepi64_si128(x1, x0, 0x10);
	xMask = _mm_setr_epi32((int)~0, 0, (int)~0, 0);
	x1 = _mm_srli_si128(x1, 8);
	x1 = _mm_xor_si128(x1, x2);

	x0 = _mm_loadl_epi64((const __m128i *)(const void *)s_k5k0);
	x2 = _mm_srli_si128(x1, 4);
	x1 = _mm_and_si128(x1, xMask);
	x1 = _mm_clmulepi64_si128(x1, x0, 0x00);
	x1 = _mm_xor_si128(x1, x2);

	/* Barrett reduction → 32-bit residual in dword 1. */
	x0 = _mm_loadu_si128((const __m128i *)(const void *)s_poly);
	x2 = _mm_and_si128(x1, xMask);
	x2 = _mm_clmulepi64_si128(x2, x0, 0x10);
	x2 = _mm_and_si128(x2, xMask);
	x2 = _mm_clmulepi64_si128(x2, x0, 0x00);
	x1 = _mm_xor_si128(x1, x2);

	/* SSE2 extract of dword 1 (avoid SSE4.1 _mm_extract_epi32). */
	return (uint32_t)_mm_cvtsi128_si32(_mm_srli_si128(x1, 4));
}

#endif /* x86 */

/* ---- public API -------------------------------------------------------- */

/*
 * Incremental IEEE CRC-32. Uses PCLMUL folding when b108_has_pclmul() and
 * len >= 16; otherwise the reflected table path (identical to batch39
 * crc32_z chaining with size_t length). NULL buf → 0.
 */
uint32_t
crc32_pclmul(uint32_t uCrc, const void *pBuf, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pBuf;

	if (p == NULL) {
		return 0u;
	}

#if defined(__x86_64__) || defined(__i386__)
	if (cb >= 16u && b108_has_pclmul() != 0) {
		size_t cbFold = cb & ~(size_t)15u;

		/* Body works in inverted domain (same as zlib simd wrappers). */
		uCrc = ~uCrc;
		uCrc = b108_crc32_pclmul_body(p, cbFold, uCrc);
		uCrc = ~uCrc;
		if (cb - cbFold != 0u) {
			uCrc = b108_crc32_sw(uCrc, p + cbFold, cb - cbFold);
		}
		return uCrc;
	}
#endif
	return b108_crc32_sw(uCrc, p, cb);
}

uint32_t __crc32_pclmul(uint32_t u, const void *p, size_t n)
    __attribute__((alias("crc32_pclmul")));
