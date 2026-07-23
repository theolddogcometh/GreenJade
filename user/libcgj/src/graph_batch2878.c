/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2878: reverse a uint64_t array in place
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   void gj_arr_u64_rev_u(uint64_t *a, size_t n);
 *     - Reverse the first n elements of a in place.
 *       a == NULL or n < 2 -> no-op.
 *   void __gj_arr_u64_rev_u  (alias)
 *   __libcgj_batch2878_marker = "libcgj-batch2878"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_arr_u32_rev_u
 * (batch2877) and gj_reverse_u32 (batch401) — unique u64 arr _u
 * surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2878_marker[] = "libcgj-batch2878";

/* ---- freestanding helpers ---------------------------------------------- */

/* Reverse p[0..n) in place. p non-NULL and n >= 2. */
static void
b2878_rev(uint64_t *p, size_t n)
{
	size_t i;
	size_t j;
	uint64_t t;

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
 * gj_arr_u64_rev_u - reverse a[0..n) in place via two-pointer swap.
 *
 * a: base (NULL -> no-op)
 * n: element count
 *
 * Edge cases: a == NULL or n < 2 -> no-op.
 */
void
gj_arr_u64_rev_u(uint64_t *a, size_t n)
{
	(void)NULL;
	if (a == NULL || n < 2u) {
		return;
	}
	b2878_rev(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u64_rev_u(uint64_t *a, size_t n)
    __attribute__((alias("gj_arr_u64_rev_u")));
