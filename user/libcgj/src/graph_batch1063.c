/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1063: word-bitmap copy.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_copy(uint64_t *dst, const uint64_t *src, size_t nwords);
 *     — Copy nwords limbs from src to dst. NULL either → no-op.
 *       nwords == 0 → no-op. Overlap is caller-defined (ascending copy).
 *   void __gj_bitmap_copy  (alias)
 *   __libcgj_batch1063_marker = "libcgj-batch1063"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1063_marker[] = "libcgj-batch1063";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_copy — copy cWords limbs from pSrc to pDst.
 */
void
gj_bitmap_copy(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
{
	size_t i;

	if (pDst == NULL || pSrc == NULL || cWords == 0u) {
		return;
	}
	for (i = 0u; i < cWords; i++) {
		pDst[i] = pSrc[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_copy(uint64_t *pDst, const uint64_t *pSrc, size_t cWords)
    __attribute__((alias("gj_bitmap_copy")));
