/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1519: pack a little-endian byte stream into
 * uint32_t words.
 *
 * Surface (unique symbols):
 *   size_t gj_bytes_to_u32_le_arr(const unsigned char *src, size_t nsrc,
 *                                 uint32_t *dst, size_t ndst);
 *     — Convert up to min(nsrc/4, ndst) little-endian 32-bit words from
 *       src into dst. Returns the number of words written. Partial
 *       trailing bytes (nsrc % 4) are ignored. NULL src/dst with work
 *       remaining → 0.
 *   size_t __gj_bytes_to_u32_le_arr  (alias)
 *   __libcgj_batch1519_marker = "libcgj-batch1519"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No real crypto copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1519_marker[] = "libcgj-batch1519";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one LE u32 from four octets (may be unaligned). */
static uint32_t
b1519_load_u32_le(const unsigned char *pB)
{
	return (uint32_t)pB[0]
	    | ((uint32_t)pB[1] << 8)
	    | ((uint32_t)pB[2] << 16)
	    | ((uint32_t)pB[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_u32_le_arr — convert LE byte stream to u32 word array.
 *
 * pSrc:  input octets (may be unaligned)
 * cbSrc: input length in bytes
 * pDst:  output word buffer
 * cDst:  capacity of pDst in words
 *
 * Returns words written (0 on no-op / bad args).
 */
size_t
gj_bytes_to_u32_le_arr(const unsigned char *pSrc, size_t cbSrc,
    uint32_t *pDst, size_t cDst)
{
	size_t cWords;
	size_t i;

	if (pSrc == NULL || pDst == NULL || cbSrc < 4u || cDst == 0u) {
		return 0u;
	}

	cWords = cbSrc / 4u;
	if (cWords > cDst) {
		cWords = cDst;
	}

	for (i = 0u; i < cWords; i++) {
		pDst[i] = b1519_load_u32_le(pSrc + (i * 4u));
	}

	return cWords;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bytes_to_u32_le_arr(const unsigned char *pSrc, size_t cbSrc,
    uint32_t *pDst, size_t cDst)
    __attribute__((alias("gj_bytes_to_u32_le_arr")));
