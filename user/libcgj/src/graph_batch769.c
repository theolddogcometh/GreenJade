/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch769: freestanding uint32_t array equality.
 *
 * Surface (unique symbols):
 *   int gj_u32_equal_arr(const uint32_t *a, const uint32_t *b, size_t n);
 *     — Return 1 if a[0..n) equals b[0..n) element-wise, else 0.
 *       n == 0 is vacuously equal (returns 1) even if a or b is NULL.
 *       If n > 0 and either pointer is NULL, returns 0.
 *   int __gj_u32_equal_arr  (alias)
 *   __libcgj_batch769_marker = "libcgj-batch769"
 *
 * Distinct from gj_mem_eq (octet windows) — element-wise uint32_t
 * compare under a unique gj_u32_equal_arr surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch769_marker[] = "libcgj-batch769";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_equal_arr — 1 if a and b hold n equal uint32_t elements.
 *
 * a, b: arrays of at least n elements when n > 0 (NULL with n > 0 → 0)
 * n:    element count (0 → 1)
 *
 * Early-exits on the first mismatch. Identical pointers with n > 0
 * short-circuit to equal.
 */
int
gj_u32_equal_arr(const uint32_t *pA, const uint32_t *pB, size_t n)
{
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (pA == NULL || pB == NULL) {
		return 0;
	}
	if (pA == pB) {
		return 1;
	}
	for (i = 0u; i < n; i++) {
		if (pA[i] != pB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_equal_arr(const uint32_t *pA, const uint32_t *pB, size_t n)
    __attribute__((alias("gj_u32_equal_arr")));
