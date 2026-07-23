/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2877: reverse a uint32_t array in place
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   void gj_arr_u32_rev_u(uint32_t *a, size_t n);
 *     - Reverse the first n elements of a in place.
 *       a == NULL or n < 2 -> no-op.
 *   void __gj_arr_u32_rev_u  (alias)
 *   __libcgj_batch2877_marker = "libcgj-batch2877"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_reverse_u32
 * (batch401) — unique arr _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2877_marker[] = "libcgj-batch2877";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse p[0..n) in place. p non-NULL and n >= 2. */
static void
b2877_rev(uint32_t *p, size_t n)
{
	size_t i;
	size_t j;
	uint32_t t;

	i = 0u;
	j = n - 1u;
	while (i < j) {
		t = p[i];
		p[i] = p[j];
		p[j] = t;
		i++;
		j--;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_rev_u - reverse a[0..n) in place via two-pointer swap.
 *
 * a: base (NULL -> no-op)
 * n: element count
 *
 * Edge cases: a == NULL or n < 2 -> no-op.
 */
void
gj_arr_u32_rev_u(uint32_t *a, size_t n)
{
	(void)NULL;
	if (a == NULL || n < 2u) {
		return;
	}
	b2877_rev(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u32_rev_u(uint32_t *a, size_t n)
    __attribute__((alias("gj_arr_u32_rev_u")));
