/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95: AVX2 bulk memory helpers (userspace).
 * Clean-room freestanding C + AVX2 intrinsics behind CPUID dispatch.
 * TU builds with -msse2 only; hot paths use __attribute__((target("avx2"))).
 * Does NOT redefine memcpy / memmove / memset (string.c) or batch84 SSE2
 * helpers (calls them as fallback when AVX2 is absent).
 *
 * Surface:
 *   int gj_cpu_has_avx2(void);  // CPUID.7:0 EBX bit 5
 *   void *gj_memcpy_avx2(void *dst, const void *src, size_t n);
 *   void *gj_memset_avx2(void *dst, int c, size_t n);
 *   __gj_cpu_has_avx2 / __gj_memcpy_avx2 / __gj_memset_avx2  (aliases)
 *   __libcgj_batch95_marker = "libcgj-batch95"
 *
 * Strategy:
 *   - Runtime CPUID leaf 7 EBX[5] (AVX2); result cached.
 *   - AVX2 path: 32-byte unaligned loads/stores (_mm256_loadu_si256 /
 *     _mm256_storeu_si256); scalar head/tail for residual bytes.
 *   - No AVX2: fall back to batch84 gj_memcpy_sse2 / gj_memset_sse2.
 *   - Kernel builds must not compile this TU (no-SSE kernel policy).
 */

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch95_marker[] = "libcgj-batch95";

/* batch84 SSE2 helpers — used when AVX2 is unavailable. */
void *gj_memcpy_sse2(void *dst, const void *src, size_t n);
void *gj_memset_sse2(void *dst, int c, size_t n);

/* ---- CPUID / AVX2 detect ----------------------------------------------- */

/* -1 unknown, 0 absent, 1 present */
static int s_b95_avx2 = -1;

static void
b95_cpuid(unsigned uLeaf, unsigned uSub, unsigned *pEax, unsigned *pEbx,
    unsigned *pEcx, unsigned *pEdx)
{
	unsigned uEax = 0u;
	unsigned uEbx = 0u;
	unsigned uEcx = 0u;
	unsigned uEdx = 0u;

#if defined(__x86_64__) || defined(__i386__)
	__asm__ volatile("cpuid"
	    : "=a"(uEax), "=b"(uEbx), "=c"(uEcx), "=d"(uEdx)
	    : "a"(uLeaf), "c"(uSub)
	    : "memory");
#else
	(void)uLeaf;
	(void)uSub;
#endif
	if (pEax != NULL) {
		*pEax = uEax;
	}
	if (pEbx != NULL) {
		*pEbx = uEbx;
	}
	if (pEcx != NULL) {
		*pEcx = uEcx;
	}
	if (pEdx != NULL) {
		*pEdx = uEdx;
	}
}

/*
 * AVX2 is CPUID.(EAX=7,ECX=0):EBX[5].
 * Returns 1 if present, 0 otherwise. Result is cached.
 */
int
gj_cpu_has_avx2(void)
{
	unsigned uEax;
	unsigned uEbx;

	if (s_b95_avx2 >= 0) {
		return s_b95_avx2;
	}
#if !(defined(__x86_64__) || defined(__i386__))
	s_b95_avx2 = 0;
	return 0;
#else
	/* Need extended feature leaf 7. */
	b95_cpuid(0u, 0u, &uEax, NULL, NULL, NULL);
	if (uEax < 7u) {
		s_b95_avx2 = 0;
		return 0;
	}
	b95_cpuid(7u, 0u, NULL, &uEbx, NULL, NULL);
	/* EBX bit 5 = AVX2 */
	s_b95_avx2 = ((uEbx & (1u << 5)) != 0u) ? 1 : 0;
	return s_b95_avx2;
#endif
}

/* ---- scalar tails / small copies --------------------------------------- */

static void
b95_copy_fwd_bytes(unsigned char *d, const unsigned char *s, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		d[i] = s[i];
	}
}

static void
b95_set_bytes(unsigned char *d, unsigned char v, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		d[i] = v;
	}
}

/* ---- AVX2 hot paths (legal only under target("avx2")) ------------------ */

/*
 * Broadcast one byte into all 32 lanes. Built from an aligned dword pattern
 * so -O0 does not invent unexpected insert paths; target("avx2") makes the
 * 256-bit load legal without -mavx2 on the TU.
 */
static __attribute__((target("avx2"))) __m256i
b95_set1_u8(unsigned char v)
{
	uint32_t w = (uint32_t)v;
	uint32_t a[8] __attribute__((aligned(32)));
	unsigned i;

	w |= w << 8;
	w |= w << 16;
	for (i = 0u; i < 8u; i++) {
		a[i] = w;
	}
	return _mm256_load_si256((const __m256i *)(const void *)a);
}

/* Forward AVX2 bulk (memcpy). Unaligned 32-byte loads/stores. */
static __attribute__((target("avx2"))) void
b95_copy_fwd_avx2(unsigned char *d, const unsigned char *s, size_t n)
{
	while (n >= 32u) {
		__m256i v = _mm256_loadu_si256((const __m256i *)(const void *)s);
		_mm256_storeu_si256((__m256i *)(void *)d, v);
		d += 32;
		s += 32;
		n -= 32u;
	}
	/* Optional 16-byte residual via AVX2-legal 128-bit lane ops. */
	if (n >= 16u) {
		__m128i v16 = _mm_loadu_si128((const __m128i *)(const void *)s);
		_mm_storeu_si128((__m128i *)(void *)d, v16);
		d += 16;
		s += 16;
		n -= 16u;
	}
	b95_copy_fwd_bytes(d, s, n);
	/* Avoid dirty upper-YMM penalty on following SSE code. */
	_mm256_zeroupper();
}

/* AVX2 bulk memset with unaligned 32-byte stores. */
static __attribute__((target("avx2"))) void
b95_set_avx2(unsigned char *d, unsigned char v, size_t n)
{
	__m256i fill = b95_set1_u8(v);

	while (n >= 32u) {
		_mm256_storeu_si256((__m256i *)(void *)d, fill);
		d += 32;
		n -= 32u;
	}
	if (n >= 16u) {
		__m128i fill16 = _mm256_castsi256_si128(fill);
		_mm_storeu_si128((__m128i *)(void *)d, fill16);
		d += 16;
		n -= 16u;
	}
	b95_set_bytes(d, v, n);
	_mm256_zeroupper();
}

/* ---- public API -------------------------------------------------------- */

void *
gj_memcpy_avx2(void *dst, const void *src, size_t n)
{
	unsigned char *d;
	const unsigned char *s;

	if (dst == NULL || src == NULL) {
		return dst;
	}
	if (n == 0u) {
		return dst;
	}

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;

	if (gj_cpu_has_avx2() != 0) {
		if (n < 32u) {
			b95_copy_fwd_bytes(d, s, n);
			return dst;
		}
		b95_copy_fwd_avx2(d, s, n);
		return dst;
	}

	/* No AVX2: prefer batch84 SSE2 path. */
	return gj_memcpy_sse2(dst, src, n);
}

void *
gj_memset_avx2(void *dst, int c, size_t n)
{
	unsigned char *d;
	unsigned char v;

	if (dst == NULL) {
		return dst;
	}
	if (n == 0u) {
		return dst;
	}

	d = (unsigned char *)dst;
	v = (unsigned char)c;

	if (gj_cpu_has_avx2() != 0) {
		if (n < 32u) {
			b95_set_bytes(d, v, n);
			return dst;
		}
		b95_set_avx2(d, v, n);
		return dst;
	}

	/* No AVX2: prefer batch84 SSE2 path. */
	return gj_memset_sse2(dst, c, n);
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_cpu_has_avx2(void)
    __attribute__((alias("gj_cpu_has_avx2")));

void *__gj_memcpy_avx2(void *dst, const void *src, size_t n)
    __attribute__((alias("gj_memcpy_avx2")));

void *__gj_memset_avx2(void *dst, int c, size_t n)
    __attribute__((alias("gj_memset_avx2")));
