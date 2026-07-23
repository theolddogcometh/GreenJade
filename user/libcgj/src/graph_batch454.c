/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch454: freestanding one-shot Adler-like 64-bit mix.
 *
 * Surface (unique symbols):
 *   uint64_t gj_adler64(const void *data, size_t n);
 *     — Dual classic Adler-32 style digests (MOD 65521, init a=1 b=0):
 *       even-indexed bytes → low 32 bits, odd-indexed → high 32 bits.
 *       Folded as ((uint64_t)odd << 32) | even.
 *       data == NULL or n == 0 → 0x0000000100000001 (both halves = 1).
 *   uint64_t __gj_adler64  (alias)
 *   __libcgj_batch454_marker = "libcgj-batch454"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch454_marker[] = "libcgj-batch454";

/* Largest prime below 2^16; same modulus as classic Adler-32. */
#define B454_ADLER_MOD 65521u

/*
 * b454_adler32_stride — Adler-32 over bytes at iStart, iStart+nStride, ...
 * Returns 1 when p is NULL (empty half matches classic empty Adler-32).
 */
static uint32_t
b454_adler32_stride(const unsigned char *p, size_t cb, size_t iStart,
    size_t nStride)
{
	uint32_t uA;
	uint32_t uB;
	size_t i;

	if (p == NULL) {
		return 1u;
	}

	uA = 1u;
	uB = 0u;
	for (i = iStart; i < cb; i += nStride) {
		uA = (uA + (uint32_t)p[i]) % B454_ADLER_MOD;
		uB = (uB + uA) % B454_ADLER_MOD;
	}
	return (uB << 16) | uA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler64 — one-shot dual Adler-32 on even/odd byte lanes, folded to 64.
 *
 * Edge cases:
 *   data == NULL → 0x0000000100000001
 *   n == 0       → 0x0000000100000001
 *   single byte  → even half updated, odd half stays 1
 */
uint64_t
gj_adler64(const void *data, size_t n)
{
	const unsigned char *pBytes;
	uint32_t uEven;
	uint32_t uOdd;

	pBytes = (const unsigned char *)data;
	uEven = b454_adler32_stride(pBytes, n, 0u, 2u);
	uOdd = b454_adler32_stride(pBytes, n, 1u, 2u);
	return ((uint64_t)uOdd << 32) | (uint64_t)uEven;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_adler64(const void *data, size_t n)
    __attribute__((alias("gj_adler64")));
