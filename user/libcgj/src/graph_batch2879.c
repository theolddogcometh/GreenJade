/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2879: swap two uint32_t elements by index
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   void gj_arr_u32_swap_u(uint32_t *a, size_t n, size_t i, size_t j);
 *     - Exchange a[i] and a[j] when both indices are in range [0, n).
 *       NULL a, n == 0, i >= n, j >= n, or i == j -> no-op (i == j is a
 *       harmless no-op even when in range).
 *   void __gj_arr_u32_swap_u  (alias)
 *   __libcgj_batch2879_marker = "libcgj-batch2879"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_u32_swap_at
 * (batch1009) — unique arr _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2879_marker[] = "libcgj-batch2879";

/* ---- freestanding helpers ---------------------------------------------- */

/* Exchange p[i] and p[j]. p non-NULL; i,j in range; i != j. */
static void
b2879_swap(uint32_t *p, size_t i, size_t j)
{
	uint32_t t;

	t = p[i];
	p[i] = p[j];
	p[j] = t;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_swap_u - exchange a[i] and a[j] when both indices are valid.
 *
 * a: base of n uint32_t elements (NULL -> no-op)
 * n: element count
 * i: first index
 * j: second index
 *
 * Bounds-checked: if either index is out of range, leaves a[] unchanged.
 */
void
gj_arr_u32_swap_u(uint32_t *a, size_t n, size_t i, size_t j)
{
	(void)NULL;
	if (a == NULL || n == 0u || i >= n || j >= n || i == j) {
		return;
	}
	b2879_swap(a, i, j);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u32_swap_u(uint32_t *a, size_t n, size_t i, size_t j)
    __attribute__((alias("gj_arr_u32_swap_u")));
