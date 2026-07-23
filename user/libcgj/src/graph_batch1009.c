/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1009: swap two uint32_t elements by index.
 *
 * Surface (unique symbols):
 *   void gj_u32_swap_at(uint32_t *a, size_t n, size_t i, size_t j);
 *     — Exchange a[i] and a[j] when both indices are in range [0, n).
 *       NULL a, n == 0, i >= n, j >= n, or i == j → no-op (i == j is a
 *       harmless no-op even when in range).
 *   void __gj_u32_swap_at  (alias)
 *   __libcgj_batch1009_marker = "libcgj-batch1009"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1009_marker[] = "libcgj-batch1009";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_swap_at — exchange a[i] and a[j] when both indices are valid.
 *
 * a: base of n uint32_t elements (NULL → no-op)
 * n: element count
 * i: first index
 * j: second index
 *
 * Bounds-checked: if either index is out of range, leaves a[] unchanged.
 */
void
gj_u32_swap_at(uint32_t *a, size_t n, size_t i, size_t j)
{
	uint32_t uTmp;

	if (a == NULL || n == 0u || i >= n || j >= n || i == j) {
		return;
	}

	uTmp = a[i];
	a[i] = a[j];
	a[j] = uTmp;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_swap_at(uint32_t *a, size_t n, size_t i, size_t j)
    __attribute__((alias("gj_u32_swap_at")));
