/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2068: freestanding count of uint32_t elements
 * strictly less than a threshold (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_u32_count_lt_n(const uint32_t *a, size_t n, uint32_t v);
 *     - Return how many elements of a[0..n) are strictly less than v.
 *       NULL a with n > 0 yields 0; n == 0 yields 0.
 *   size_t __gj_u32_count_lt_n  (alias)
 *   __libcgj_batch2068_marker = "libcgj-batch2068"
 *
 * Distinct from gj_u32_count_eq_n (batch2067) and gj_u32_count
 * (batch612) - unique gj_u32_count_lt_n name only; no multi-def.
 * Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2068_marker[] = "libcgj-batch2068";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count a[i] < u32V over a[0..cN). Caller: pA non-NULL when cN > 0. */
static size_t
b2068_count_lt(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;
	size_t cHit;

	cHit = 0u;
	for (i = 0u; i < cN; i++) {
		if (pA[i] < u32V) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_count_lt_n - count elements strictly less than v.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 * v: exclusive upper bound for the comparison
 *
 * Returns the number of a[i] with a[i] < v in a[0..n).
 */
size_t
gj_u32_count_lt_n(const uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2068_count_lt(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_count_lt_n(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_count_lt_n")));
