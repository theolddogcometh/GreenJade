/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch607: bounded all-zero memory predicate.
 *
 * Surface (unique symbols):
 *   int gj_mem_is_zero(const void *p, size_t n);
 *     — Return 1 if every octet in p[0..n) is zero, else 0. n == 0 is
 *       vacuously all-zero (returns 1) even if p is NULL. If n > 0 and
 *       p is NULL, returns 0.
 *   int __gj_mem_is_zero  (alias)
 *   __libcgj_batch607_marker = "libcgj-batch607"
 *
 * Distinct from gj_mem_eq / gj_mem_cmp and libc memset/memcmp — unique
 * gj_ names only; no multi-def with the existing string graph.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch607_marker[] = "libcgj-batch607";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_is_zero — true if p[0..n) is all zero octets.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 → 0)
 * n: number of octets to scan (0 → 1, empty window is vacuously zero)
 *
 * Does not call libc. Early-exits on the first non-zero byte.
 */
int
gj_mem_is_zero(const void *p, size_t n)
{
	const unsigned char *pu;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (p == NULL) {
		return 0;
	}

	pu = (const unsigned char *)p;
	for (i = 0u; i < n; i++) {
		if (pu[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_is_zero(const void *p, size_t n)
    __attribute__((alias("gj_mem_is_zero")));
