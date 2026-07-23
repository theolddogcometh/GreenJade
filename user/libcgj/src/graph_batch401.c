/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch401: reverse a uint32_t array in place.
 *
 * Surface (unique symbols):
 *   void gj_reverse_u32(uint32_t *a, size_t n);
 *     — Reverse the first n elements of a in place.
 *       a == NULL or n < 2 → no-op.
 *   void __gj_reverse_u32  (alias)
 *   __libcgj_batch401_marker = "libcgj-batch401"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch401_marker[] = "libcgj-batch401";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_reverse_u32 — reverse a[0..n) in place via two-pointer swap.
 *
 * Edge cases:
 *   a == NULL → no-op
 *   n < 2 → no-op (nothing to reverse)
 */
void
gj_reverse_u32(uint32_t *a, size_t n)
{
	size_t i;
	size_t j;
	uint32_t t;

	if (a == NULL || n < 2u) {
		return;
	}

	i = 0u;
	j = n - 1u;
	while (i < j) {
		t = a[i];
		a[i] = a[j];
		a[j] = t;
		i++;
		j--;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_reverse_u32(uint32_t *a, size_t n)
    __attribute__((alias("gj_reverse_u32")));
