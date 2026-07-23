/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1061: in-place word-bitmap XOR.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_xor_into(uint64_t *dst, const uint64_t *src, size_t nwords);
 *     — For each i in [0, nwords): dst[i] ^= src[i].
 *       NULL dst or NULL src → no-op. nwords == 0 → no-op.
 *   void __gj_bitmap_xor_into  (alias)
 *   __libcgj_batch1061_marker = "libcgj-batch1061"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1061_marker[] = "libcgj-batch1061";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_xor_into — XOR src limbs into dst limbs in place.
 *
 * pDst:   destination word array (NULL → no-op)
 * pSrc:   source word array (NULL → no-op)
 * cWords: number of uint64_t limbs
 */
void
gj_bitmap_xor_into(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	size_t i;

	if (pDst == NULL || pSrc == NULL || cWords == 0u) {
		return;
	}
	for (i = 0u; i < cWords; i++) {
		pDst[i] ^= pSrc[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_xor_into(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
    __attribute__((alias("gj_bitmap_xor_into")));
