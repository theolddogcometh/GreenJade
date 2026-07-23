/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch269: streaming Adler-32 (zlib-shaped).
 *
 * Surface (unique symbols — does NOT redefine one-shot adler32/adler32_z):
 *   uint32_t gj_adler32_init(void);
 *     — Returns the canonical initial checksum value 1.
 *   uint32_t gj_adler32_update(uint32_t adler, const void *data, size_t n);
 *     — Fold n bytes into adler. data == NULL → 1 (zlib-shaped probe).
 *       n == 0 with non-NULL data is a no-op (returns adler unchanged).
 *   uint32_t gj_adler32_final(uint32_t adler);
 *     — Identity: Adler-32 needs no padding/finalize step.
 *   __gj_adler32_init / __gj_adler32_update / __gj_adler32_final  (aliases)
 *   __libcgj_batch269_marker = "libcgj-batch269"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from graph_batch39 one-shot.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch269_marker[] = "libcgj-batch269";

/* Largest n where 255*n*(n+1)/2 + (n+1)*(BASE-1) fits in 32 bits (zlib). */
#define B269_NMAX 5552u
/* Adler-32 prime modulus. */
#define B269_BASE 65521u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_init — streaming start value (s1=1, s2=0 → packed 1).
 */
uint32_t
gj_adler32_init(void)
{
	return 1u;
}

/*
 * gj_adler32_update — accumulate n bytes into a running Adler-32.
 *
 * Encoding matches zlib/ISO 3309 Adler-32:
 *   s1 = adler & 0xffff
 *   s2 = (adler >> 16) & 0xffff
 *   for each byte b: s1 = (s1 + b) % BASE; s2 = (s2 + s1) % BASE
 *   result = (s2 << 16) | s1
 *
 * Deferred modulo in blocks of B269_NMAX keeps intermediates in uint32_t.
 * data == NULL returns 1 (same probe convention as zlib adler32 / batch39).
 */
uint32_t
gj_adler32_update(uint32_t uAdler, const void *pData, size_t cb)
{
	const unsigned char *pBytes;
	uint32_t uS1;
	uint32_t uS2;
	size_t cbLeft;
	size_t cbChunk;
	size_t iByte;

	if (pData == NULL) {
		return 1u;
	}
	if (cb == 0u) {
		return uAdler;
	}

	pBytes = (const unsigned char *)pData;
	uS1 = uAdler & 0xffffu;
	uS2 = (uAdler >> 16) & 0xffffu;
	cbLeft = cb;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B269_NMAX) {
			cbChunk = (size_t)B269_NMAX;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			uS1 += (uint32_t)pBytes[iByte];
			uS2 += uS1;
		}
		pBytes += cbChunk;
		cbLeft -= cbChunk;
		uS1 %= B269_BASE;
		uS2 %= B269_BASE;
	}

	return (uS2 << 16) | uS1;
}

/*
 * gj_adler32_final — identity finalize for streaming use.
 * Adler-32 has no length padding or residual block; the running value
 * from gj_adler32_update is already the complete checksum.
 */
uint32_t
gj_adler32_final(uint32_t uAdler)
{
	return uAdler;
}

uint32_t __gj_adler32_init(void)
    __attribute__((alias("gj_adler32_init")));
uint32_t __gj_adler32_update(uint32_t uAdler, const void *pData, size_t cb)
    __attribute__((alias("gj_adler32_update")));
uint32_t __gj_adler32_final(uint32_t uAdler)
    __attribute__((alias("gj_adler32_final")));
