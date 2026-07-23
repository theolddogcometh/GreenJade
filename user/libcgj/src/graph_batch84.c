/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch84: SSE2 bulk memory helpers (userspace).
 * Clean-room freestanding C + SSE2 intrinsics (-msse2).
 * Does NOT redefine memcpy / memmove / memset (those live in string.c).
 *
 * Surface:
 *   void *gj_memcpy_sse2(void *dst, const void *src, size_t n);
 *   void *gj_memset_sse2(void *dst, int c, size_t n);
 *   void *gj_memmove_sse2(void *dst, const void *src, size_t n);
 *   __gj_memcpy_sse2 / __gj_memset_sse2 / __gj_memmove_sse2  (aliases)
 *   __libcgj_batch84_marker = "libcgj-batch84"
 *
 * Strategy:
 *   - Scalar head/tail for sub-16-byte work and residual bytes.
 *   - Bulk path uses SSE2 16-byte unaligned loads/stores
 *     (_mm_loadu_si128 / _mm_storeu_si128) so all alignments are correct.
 *   - memmove: forward when dst <= src (or no overlap); backward when
 *     regions overlap with dst > src.
 *   - Kernel builds must not compile this TU (no-SSE kernel policy).
 */

#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch84_marker[] = "libcgj-batch84";

/* ---- pure SSE2 helpers ------------------------------------------------- */

/*
 * Broadcast one byte into all 16 lanes using only SSE2 ops.
 * Built via an aligned dword pattern + movdqa so gcc -O0 does not
 * invent SSE4.1 pinsr* (as it can for _mm_set1_epi8 / cvtsi32 paths).
 */
static __m128i
b84_set1_u8(unsigned char v)
{
	uint32_t w = (uint32_t)v;
	uint32_t a[4] __attribute__((aligned(16)));

	w |= w << 8;
	w |= w << 16;
	a[0] = w;
	a[1] = w;
	a[2] = w;
	a[3] = w;
	return _mm_load_si128((const __m128i *)(const void *)a);
}

/* ---- scalar tails / small copies --------------------------------------- */

static void
b84_copy_fwd_bytes(unsigned char *d, const unsigned char *s, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		d[i] = s[i];
	}
}

static void
b84_copy_bwd_bytes(unsigned char *d, const unsigned char *s, size_t n)
{
	while (n > 0u) {
		n--;
		d[n] = s[n];
	}
}

static void
b84_set_bytes(unsigned char *d, unsigned char v, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		d[i] = v;
	}
}

/* ---- forward SSE2 bulk (memcpy / non-overlapping memmove) -------------- */

static void
b84_copy_fwd_sse2(unsigned char *d, const unsigned char *s, size_t n)
{
	/* Process 16-byte chunks with unaligned SSE2 loads/stores. */
	while (n >= 16u) {
		__m128i v = _mm_loadu_si128((const __m128i *)(const void *)s);
		_mm_storeu_si128((__m128i *)(void *)d, v);
		d += 16;
		s += 16;
		n -= 16u;
	}
	b84_copy_fwd_bytes(d, s, n);
}

/* ---- backward SSE2 bulk (overlapping memmove, dst > src) --------------- */

static void
b84_copy_bwd_sse2(unsigned char *d, const unsigned char *s, size_t n)
{
	/*
	 * Walk from the high end so overlapping regions stay correct.
	 * Unaligned 16-byte load/store of the trailing window each step.
	 */
	while (n >= 16u) {
		n -= 16u;
		{
			__m128i v =
			    _mm_loadu_si128((const __m128i *)(const void *)(s + n));
			_mm_storeu_si128((__m128i *)(void *)(d + n), v);
		}
	}
	b84_copy_bwd_bytes(d, s, n);
}

/* ---- public API -------------------------------------------------------- */

void *
gj_memcpy_sse2(void *dst, const void *src, size_t n)
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

	if (n < 16u) {
		b84_copy_fwd_bytes(d, s, n);
		return dst;
	}

	b84_copy_fwd_sse2(d, s, n);
	return dst;
}

void *
gj_memset_sse2(void *dst, int c, size_t n)
{
	unsigned char *d;
	unsigned char v;
	__m128i fill;

	if (dst == NULL) {
		return dst;
	}
	if (n == 0u) {
		return dst;
	}

	d = (unsigned char *)dst;
	v = (unsigned char)c;

	if (n < 16u) {
		b84_set_bytes(d, v, n);
		return dst;
	}

	fill = b84_set1_u8(v);

	/* Align destination to 16 bytes with a short scalar head. */
	{
		uintptr_t mis = ((uintptr_t)d) & 15u;

		if (mis != 0u) {
			size_t head = 16u - mis;

			if (head > n) {
				head = n;
			}
			b84_set_bytes(d, v, head);
			d += head;
			n -= head;
		}
	}

	/* Aligned 16-byte stores for the bulk body. */
	while (n >= 16u) {
		_mm_store_si128((__m128i *)(void *)d, fill);
		d += 16;
		n -= 16u;
	}

	b84_set_bytes(d, v, n);
	return dst;
}

void *
gj_memmove_sse2(void *dst, const void *src, size_t n)
{
	unsigned char *d;
	const unsigned char *s;
	uintptr_t ud;
	uintptr_t us;

	if (dst == NULL || src == NULL) {
		return dst;
	}
	if (n == 0u) {
		return dst;
	}

	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	ud = (uintptr_t)d;
	us = (uintptr_t)s;

	if (ud == us) {
		return dst;
	}

	/*
	 * Forward is safe when dst is at or below src, or when the ranges
	 * do not overlap (dst >= src + n). Otherwise copy backward.
	 */
	if (ud < us || ud >= us + n) {
		if (n < 16u) {
			b84_copy_fwd_bytes(d, s, n);
		} else {
			b84_copy_fwd_sse2(d, s, n);
		}
	} else {
		if (n < 16u) {
			b84_copy_bwd_bytes(d, s, n);
		} else {
			b84_copy_bwd_sse2(d, s, n);
		}
	}

	return dst;
}

/* ---- aliases ----------------------------------------------------------- */

void *__gj_memcpy_sse2(void *dst, const void *src, size_t n)
    __attribute__((alias("gj_memcpy_sse2")));

void *__gj_memset_sse2(void *dst, int c, size_t n)
    __attribute__((alias("gj_memset_sse2")));

void *__gj_memmove_sse2(void *dst, const void *src, size_t n)
    __attribute__((alias("gj_memmove_sse2")));
