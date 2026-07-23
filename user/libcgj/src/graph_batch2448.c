/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2448: in-place bitmap OR (dst |= src), _u form.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_or_into_u(uint64_t *dst, const uint64_t *src,
 *                            size_t nwords);
 *     — For each word i in [0, nwords): dst[i] |= src[i]. No-op if dst
 *       or src is NULL or nwords == 0. Operates on full 64-bit limbs
 *       (no partial-bit mask).
 *   void __gj_bitmap_or_into_u  (alias)
 *   __libcgj_batch2448_marker = "libcgj-batch2448"
 *
 * Bitmap exclusive wave (2441-2450). Distinct from gj_bitmap_or_into
 * (batch1059) — unique gj_bitmap_or_into_u name only. Pair with
 * gj_bitmap_and_into_u (batch2449).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2448_marker[] = "libcgj-batch2448";

/* ---- freestanding helpers ---------------------------------------------- */

/* dst[i] |= src[i] for i in [0, cWords). */
static void
b2448_or_into(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		pDst[iWord] |= pSrc[iWord];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_or_into_u — dst[i] |= src[i] for i in [0, nwords).
 *
 * dst:    mutable destination limbs
 * src:    source limbs (read-only)
 * nwords: number of full uint64_t limbs
 *
 * No-op when dst or src is NULL or nwords == 0. Does not call libc.
 */
void
gj_bitmap_or_into_u(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	if (pDst == NULL || pSrc == NULL || cWords == 0u) {
		return;
	}
	b2448_or_into(pDst, pSrc, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_or_into_u(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
    __attribute__((alias("gj_bitmap_or_into_u")));
