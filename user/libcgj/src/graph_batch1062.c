/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1062: in-place word-bitmap AND-NOT.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_andnot_into(uint64_t *dst, const uint64_t *src, size_t nwords);
 *     — For each i in [0, nwords): dst[i] &= ~src[i].
 *       NULL dst or NULL src → no-op. nwords == 0 → no-op.
 *   void __gj_bitmap_andnot_into  (alias)
 *   __libcgj_batch1062_marker = "libcgj-batch1062"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1062_marker[] = "libcgj-batch1062";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_andnot_into — clear bits in dst that are set in src.
 *
 * pDst:   destination word array (NULL → no-op)
 * pSrc:   source word array of bits to clear (NULL → no-op)
 * cWords: number of uint64_t limbs
 */
void
gj_bitmap_andnot_into(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	size_t i;

	if (pDst == NULL || pSrc == NULL || cWords == 0u) {
		return;
	}
	for (i = 0u; i < cWords; i++) {
		pDst[i] &= ~pSrc[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_andnot_into(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
    __attribute__((alias("gj_bitmap_andnot_into")));
