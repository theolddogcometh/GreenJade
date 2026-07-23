/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2069: freestanding any-equal predicate over
 * a uint32_t array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_u32_any_eq_n(const uint32_t *a, size_t n, uint32_t v);
 *     - Return 1 if any element of a[0..n) equals v, else 0.
 *       NULL a with n > 0 yields 0; n == 0 yields 0. Early-exits on
 *       the first match.
 *   int __gj_u32_any_eq_n  (alias)
 *   __libcgj_batch2069_marker = "libcgj-batch2069"
 *
 * Distinct from gj_u32_count_eq_n (batch2067 full count) and
 * gj_u32_find (batch611 index) - unique gj_u32_any_eq_n predicate
 * only; no multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2069_marker[] = "libcgj-batch2069";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if any a[i] == u32V. Caller guarantees pA non-NULL when cN > 0. */
static int
b2069_any_eq(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_any_eq_n - true if any element equals v.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 * v: value to match
 *
 * Returns 1 on first match, else 0. Does not call libc.
 */
int
gj_u32_any_eq_n(const uint32_t *a, size_t n, uint32_t v)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0;
	}
	return b2069_any_eq(a, n, v);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_any_eq_n(const uint32_t *a, size_t n, uint32_t v)
    __attribute__((alias("gj_u32_any_eq_n")));
