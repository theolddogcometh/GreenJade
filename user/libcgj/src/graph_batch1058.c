/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1058: in-place bitmap AND (dst &= src).
 *
 * Surface (unique symbols):
 *   void gj_bitmap_and_into(uint64_t *dst, const uint64_t *src, size_t nwords);
 *     — For each word i in [0, nwords): dst[i] &= src[i]. No-op if dst
 *       or src is NULL or nwords == 0. Operates on full 64-bit limbs
 *       (no partial-bit mask).
 *   void __gj_bitmap_and_into  (alias)
 *   __libcgj_batch1058_marker = "libcgj-batch1058"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1058_marker[] = "libcgj-batch1058";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_and_into — dst[i] &= src[i] for i in [0, nwords).
 */
void
gj_bitmap_and_into(uint64_t *dst, const uint64_t *src, size_t nwords)
{
	size_t i;

	if (dst == NULL || src == NULL || nwords == 0u) {
		return;
	}

	for (i = 0u; i < nwords; i++) {
		dst[i] &= src[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_and_into(uint64_t *dst, const uint64_t *src, size_t nwords)
    __attribute__((alias("gj_bitmap_and_into")));
