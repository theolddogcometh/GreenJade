/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2872: count equal uint32_t values in a bounded
 * array (_u arr surface).
 *
 * Surface (unique symbols):
 *   size_t gj_arr_u32_count_eq_u(const uint32_t *a, size_t n, uint32_t v);
 *     - Return how many elements of a[0..n) equal v. NULL a with n > 0
 *       yields 0; n == 0 yields 0.
 *   size_t __gj_arr_u32_count_eq_u  (alias)
 *   __libcgj_batch2872_marker = "libcgj-batch2872"
 *
 * Array exclusive wave (2871-2880). Distinct from gj_count_eq_u32
 * (batch440) — unique arr _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2872_marker[] = "libcgj-batch2872";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count elements equal to v in p[0..n). p non-NULL when n > 0. */
static size_t
b2872_count_eq(const uint32_t *p, size_t n, uint32_t v)
{
	size_t i;
	size_t cHit;

	cHit = 0u;
	for (i = 0u; i < n; i++) {
		if (p[i] == v) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_count_eq_u - count elements equal to v.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 * v: value to match
 *
 * Returns the number of matches in a[0..n).
 */
size_t
gj_arr_u32_count_eq_u(const uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2872_count_eq(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arr_u32_count_eq_u(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_arr_u32_count_eq_u")));
