/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2067: freestanding count of matching
 * uint32_t values in a bounded array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_u32_count_eq_n(const uint32_t *a, size_t n, uint32_t v);
 *     - Return how many elements of a[0..n) equal v. NULL a with n > 0
 *       yields 0; n == 0 yields 0.
 *   size_t __gj_u32_count_eq_n  (alias)
 *   __libcgj_batch2067_marker = "libcgj-batch2067"
 *
 * Distinct from gj_u32_count (batch612) and gj_count_eq_u32 (batch440)
 * - unique gj_u32_count_eq_n name only; no multi-def. Array u32
 * exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2067_marker[] = "libcgj-batch2067";

/* ---- freestanding helpers ---------------------------------------------- */

/* Count equals of u32V in a[0..cN). Caller: pA non-NULL when cN > 0. */
static size_t
b2067_count_eq(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;
	size_t cHit;

	cHit = 0u;
	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			cHit++;
		}
	}
	return cHit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_count_eq_n - count elements equal to v.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 * v: value to match
 *
 * Returns the number of matches in a[0..n).
 */
size_t
gj_u32_count_eq_n(const uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2067_count_eq(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_count_eq_n(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_count_eq_n")));
