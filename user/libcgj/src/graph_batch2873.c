/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2873: fill a uint32_t array with a constant
 * (_u arr surface).
 *
 * Surface (unique symbols):
 *   void gj_arr_u32_fill_u(uint32_t *a, size_t n, uint32_t v);
 *     - Write v into each of the first n elements of a.
 *       a == NULL or n == 0 -> no-op.
 *   void __gj_arr_u32_fill_u  (alias)
 *   __libcgj_batch2873_marker = "libcgj-batch2873"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_fill_u32 (batch402)
 * — unique arr _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2873_marker[] = "libcgj-batch2873";

/* ---- freestanding helpers ---------------------------------------------- */

/* Set p[i] = v for i in [0, n). p non-NULL when n > 0. */
static void
b2873_fill(uint32_t *p, size_t n, uint32_t v)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		p[i] = v;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_fill_u - set a[i] = v for i in [0, n).
 *
 * a: base (NULL -> no-op)
 * n: element count
 * v: fill value
 *
 * Edge cases: a == NULL or n == 0 -> no-op.
 */
void
gj_arr_u32_fill_u(uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (a == NULL || n == 0u) {
		return;
	}
	b2873_fill(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arr_u32_fill_u(uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_arr_u32_fill_u")));
