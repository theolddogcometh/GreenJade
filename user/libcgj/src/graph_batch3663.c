/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3663: Bloom filter bit test (_u).
 *
 * Surface (unique symbols):
 *   int gj_bloom_test_bit_u(const unsigned char *bits, size_t nbytes,
 *                           size_t bit);
 *     - Return 1 if bit `bit` is set in the bit array of nbytes bytes,
 *       else 0. bit is a global bit index (byte = bit>>3, mask =
 *       1<<(bit&7)). Out of range (bit >= nbytes*8), NULL bits, or
 *       nbytes == 0 → 0.
 *   int __gj_bloom_test_bit_u  (alias)
 *   __libcgj_batch3663_marker = "libcgj-batch3663"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Distinct from
 * gj_bloom_may_contain (batch125) — unique raw bit-test _u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3663_marker[] = "libcgj-batch3663";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Test bit uBit in pBits[0..cb). Caller guarantees pBits non-NULL,
 * cb > 0, and uBit < cb*8.
 */
static int
b3663_test(const unsigned char *pBits, size_t uBit)
{
	return (pBits[uBit >> 3] &
	        (unsigned char)(1u << (uBit & 7u))) != 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_test_bit_u - 1 if bit is set in bits[0..nbytes), else 0.
 *
 * bits:   bit array (may be NULL → 0)
 * nbytes: size of bits in bytes
 * bit:    absolute bit index (0 = LSB of bits[0])
 *
 * Does not call libc. No parent wires.
 */
int
gj_bloom_test_bit_u(const unsigned char *pBits, size_t cBytes, size_t uBit)
{
	size_t nBits;

	(void)NULL;
	if (pBits == NULL || cBytes == 0u) {
		return 0;
	}
	nBits = cBytes * 8u;
	if (uBit >= nBits) {
		return 0;
	}
	return b3663_test(pBits, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bloom_test_bit_u(const unsigned char *pBits, size_t cBytes,
                          size_t uBit)
    __attribute__((alias("gj_bloom_test_bit_u")));
