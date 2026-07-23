/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2862: in-place word-bitmap NOT / invert (_u).
 *
 * Surface (unique symbols):
 *   void gj_bitmap_not_u(uint64_t *dst, size_t nwords);
 *     — For each word i in [0, nwords): dst[i] = ~dst[i]. No-op if dst
 *       is NULL or nwords == 0. Full 64-bit limbs (no partial mask).
 *   void __gj_bitmap_not_u  (alias)
 *   __libcgj_batch2862_marker = "libcgj-batch2862"
 *
 * Bitmap/array exclusive wave (2861-2870). Unique gj_bitmap_not_u
 * surface (no prior invert twin on the gj_bitmap_* catalog). Pair with
 * gj_bitmap_xor_u (batch2861).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2862_marker[] = "libcgj-batch2862";

/* ---- freestanding helpers ---------------------------------------------- */

/* dst[i] = ~dst[i] for i in [0, cWords). */
static void
b2862_not(uint64_t *pDst, size_t cWords)
{
	size_t iWord;

	for (iWord = 0u; iWord < cWords; iWord++) {
		pDst[iWord] = ~pDst[iWord];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_not_u — invert every bit in dst[0..nwords).
 *
 * dst:    mutable destination limbs
 * nwords: number of full uint64_t limbs
 *
 * No-op when dst is NULL or nwords == 0. Does not call libc.
 */
void
gj_bitmap_not_u(uint64_t *pDst, size_t cWords)
{
	(void)NULL;
	if (pDst == NULL || cWords == 0u) {
		return;
	}
	b2862_not(pDst, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_not_u(uint64_t *pDst, size_t cWords)
    __attribute__((alias("gj_bitmap_not_u")));
