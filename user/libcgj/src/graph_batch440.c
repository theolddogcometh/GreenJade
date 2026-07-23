/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch440: freestanding count of equal uint32_t
 * values in a bounded array.
 *
 * Surface (unique symbols):
 *   size_t gj_count_eq_u32(const uint32_t *a, size_t n, uint32_t v);
 *     — Return how many elements of a[0..n) equal v. NULL a with n > 0
 *       yields 0; n == 0 yields 0.
 *   size_t __gj_count_eq_u32  (alias)
 *   __libcgj_batch440_marker = "libcgj-batch440"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch440_marker[] = "libcgj-batch440";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_count_eq_u32 — count elements equal to v.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 * v: value to match
 *
 * Returns the number of matches in a[0..n).
 */
size_t
gj_count_eq_u32(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;
	size_t cHit;

	if (cN == 0u || pA == NULL) {
		return 0u;
	}

	cHit = 0u;
	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_count_eq_u32(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_count_eq_u32")));
