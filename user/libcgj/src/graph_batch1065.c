/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1065: zero a word-bitmap.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_zero(uint64_t *dst, size_t nwords);
 *     — Set every limb of dst[0..nwords) to 0. NULL dst or nwords == 0
 *       → no-op.
 *   void __gj_bitmap_zero  (alias)
 *   __libcgj_batch1065_marker = "libcgj-batch1065"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1065_marker[] = "libcgj-batch1065";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_zero — clear all bits in a dense word bitmap.
 */
void
gj_bitmap_zero(uint64_t *pDst, size_t cWords)
{
	size_t i;

	if (pDst == NULL || cWords == 0u) {
		return;
	}
	for (i = 0u; i < cWords; i++) {
		pDst[i] = 0ull;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_zero(uint64_t *pDst, size_t cWords)
    __attribute__((alias("gj_bitmap_zero")));
