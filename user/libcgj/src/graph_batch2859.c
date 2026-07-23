/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2859: three-operand bitmap AND (dst = a & b).
 *
 * Surface (unique symbols):
 *   void gj_bitmap_and_u(uint64_t *dst, const uint64_t *a,
 *                        const uint64_t *b, size_t nwords);
 *     — For each word i in [0, nwords): dst[i] = a[i] & b[i]. No-op if
 *       dst, a, or b is NULL or nwords == 0. Operates on full 64-bit
 *       limbs (no partial-bit mask). Overlapping buffers: when dst
 *       aliases a or b, result is still defined (word-wise assign).
 *   void __gj_bitmap_and_u  (alias)
 *   __libcgj_batch2859_marker = "libcgj-batch2859"
 *
 * Bitmap exclusive wave (2851-2860). Distinct from gj_bitmap_and_into
 * (batch1058) and gj_bitmap_and_into_u (batch2449) — unique three-operand
 * gj_bitmap_and_u surface. Pair with gj_bitmap_or_u (batch2858).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2859_marker[] = "libcgj-batch2859";

/* ---- freestanding helpers ---------------------------------------------- */

/* dst[i] = a[i] & b[i] for i in [0, cWords). */
static void
b2859_and(uint64_t *pDst, const uint64_t *pA, const uint64_t *pB,
    size_t cWords)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		pDst[iWord] = pA[iWord] & pB[iWord];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_and_u — dst[i] = a[i] & b[i] for i in [0, nwords).
 *
 * dst:    mutable destination limbs
 * a, b:   source limbs (read-only)
 * nwords: number of full uint64_t limbs
 *
 * No-op when dst, a, or b is NULL or nwords == 0. Does not call libc.
 */
void
gj_bitmap_and_u(uint64_t *pDst, const uint64_t *pA, const uint64_t *pB,
    size_t cWords)
{
	if (pDst == NULL || pA == NULL || pB == NULL || cWords == 0u) {
		return;
	}
	b2859_and(pDst, pA, pB, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_and_u(uint64_t *pDst, const uint64_t *pA, const uint64_t *pB,
    size_t cWords) __attribute__((alias("gj_bitmap_and_u")));
