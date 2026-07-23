/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch783: one-shot Adler-32 over n bytes.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_n(const void *data, size_t n);
 *     — Adler-32 of n bytes starting at data (init s1=1, s2=0).
 *       data == NULL → 1 (zlib-shaped probe). n == 0 with non-NULL → 1.
 *   uint32_t __gj_adler32_n  (alias)
 *   __libcgj_batch783_marker = "libcgj-batch783"
 *
 * Distinct from gj_adler32_init/update/final (batch269) and one-shot
 * adler32 (batch39) — unique _n name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch783_marker[] = "libcgj-batch783";

#define B783_NMAX 5552u
#define B783_BASE 65521u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_n — one-shot Adler-32 over n bytes.
 *
 * Encoding: s1 = 1, s2 = 0; for each byte b:
 *   s1 = (s1 + b) % BASE; s2 = (s2 + s1) % BASE
 * result = (s2 << 16) | s1
 */
uint32_t
gj_adler32_n(const void *pData, size_t cb)
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
		return 1u;
	}

	pBytes = (const unsigned char *)pData;
	uS1 = 1u;
	uS2 = 0u;
	cbLeft = cb;

	while (cbLeft > 0u) {
		cbChunk = cbLeft;
		if (cbChunk > (size_t)B783_NMAX) {
			cbChunk = (size_t)B783_NMAX;
		}
		for (iByte = 0u; iByte < cbChunk; iByte++) {
			uS1 += (uint32_t)pBytes[iByte];
			uS2 += uS1;
		}
		pBytes += cbChunk;
		cbLeft -= cbChunk;
		uS1 %= B783_BASE;
		uS2 %= B783_BASE;
	}

	return (uS2 << 16) | uS1;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_n(const void *pData, size_t cb)
    __attribute__((alias("gj_adler32_n")));
