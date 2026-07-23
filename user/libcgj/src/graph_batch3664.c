/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3664: Bloom filter bit set (_u).
 *
 * Surface (unique symbols):
 *   void gj_bloom_set_bit_u(unsigned char *bits, size_t nbytes,
 *                           size_t bit);
 *     - Set bit `bit` in the bit array of nbytes bytes (OR with
 *       1<<(bit&7) at bits[bit>>3]). No-op if bits is NULL, nbytes
 *       is 0, or bit is out of range (bit >= nbytes*8).
 *   void __gj_bloom_set_bit_u  (alias)
 *   __libcgj_batch3664_marker = "libcgj-batch3664"
 *
 * Exclusive continuum CREATE-ONLY (3661-3670). Companion to
 * gj_bloom_test_bit_u (batch3663). Distinct from gj_bloom_add
 * (batch125) — unique raw bit-set _u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3664_marker[] = "libcgj-batch3664";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Set bit uBit in pBits. Caller guarantees pBits non-NULL and
 * uBit in range.
 */
static void
b3664_set(unsigned char *pBits, size_t uBit)
{
	pBits[uBit >> 3] |= (unsigned char)(1u << (uBit & 7u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bloom_set_bit_u - set one bit in bits[0..nbytes).
 *
 * bits:   mutable bit array (may be NULL → no-op)
 * nbytes: size of bits in bytes
 * bit:    absolute bit index (0 = LSB of bits[0])
 *
 * Does not call libc. No parent wires.
 */
void
gj_bloom_set_bit_u(unsigned char *pBits, size_t cBytes, size_t uBit)
{
	size_t nBits;

	(void)NULL;
	if (pBits == NULL || cBytes == 0u) {
		return;
	}
	nBits = cBytes * 8u;
	if (uBit >= nBits) {
		return;
	}
	b3664_set(pBits, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bloom_set_bit_u(unsigned char *pBits, size_t cBytes, size_t uBit)
    __attribute__((alias("gj_bloom_set_bit_u")));
