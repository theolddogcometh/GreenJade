/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch948: index of maximum uint32 in array.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_argmax(const uint32_t *a, size_t n);
 *     — Return index of first maximum element. n==0 or a==NULL → 0.
 *   size_t __gj_u32_argmax  (alias)
 *   __libcgj_batch948_marker = "libcgj-batch948"

 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch948_marker[] = "libcgj-batch948";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_argmax — index of the (first) maximum value in a[0..n).
 */
size_t
gj_u32_argmax(const uint32_t *pA, size_t cN)
{
	size_t iBest;
	size_t i;
	uint32_t u32Best;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	iBest = 0u;
	u32Best = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] > u32Best) {
			u32Best = pA[i];
			iBest = i;
		}
	}
	return iBest;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_argmax(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_argmax")));
