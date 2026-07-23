/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2861: in-place word-bitmap XOR (_u).
 *
 * Surface (unique symbols):
 *   void gj_bitmap_xor_u(uint64_t *dst, const uint64_t *src, size_t nwords);
 *     — For each word i in [0, nwords): dst[i] ^= src[i]. No-op if dst
 *       or src is NULL or nwords == 0. Full 64-bit limbs (no partial mask).
 *   void __gj_bitmap_xor_u  (alias)
 *   __libcgj_batch2861_marker = "libcgj-batch2861"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from
 * gj_bitmap_xor_into (batch1061) — unique gj_bitmap_xor_u name only.
 * Pair with gj_bitmap_not_u (batch2862).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2861_marker[] = "libcgj-batch2861";

/* ---- freestanding helpers ---------------------------------------------- */

/* dst[i] ^= src[i] for i in [0, cWords). */
static void
b2861_xor(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		pDst[iWord] ^= pSrc[iWord];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_xor_u — XOR src limbs into dst limbs in place.
 *
 * dst:    mutable destination limbs
 * src:    source limbs (read-only)
 * nwords: number of full uint64_t limbs
 *
 * No-op when dst or src is NULL or nwords == 0. Does not call libc.
 */
void
gj_bitmap_xor_u(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	(void)NULL;
	if (pDst == NULL || pSrc == NULL || cWords == 0u) {
		return;
	}
	b2861_xor(pDst, pSrc, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_xor_u(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
    __attribute__((alias("gj_bitmap_xor_u")));
