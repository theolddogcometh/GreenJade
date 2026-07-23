/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2749: freestanding all-zero n-byte predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_mem_is_zero_n_u(const void *p, size_t n);
 *     - Return 1 if every octet in p[0..n) is zero, else 0. n == 0 is
 *       vacuously all-zero (returns 1) even if p is NULL. If n > 0 and
 *       p is NULL, returns 0.
 *   int __gj_mem_is_zero_n_u  (alias)
 *   __libcgj_batch2749_marker = "libcgj-batch2749"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_mem_is_zero_n
 * (batch2021) and gj_mem_is_zero (batch607) — unique
 * gj_mem_is_zero_n_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2749_marker[] = "libcgj-batch2749";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if pU[0..n) is all zero octets. Caller guarantees p non-NULL when n>0. */
static int
b2749_is_zero(const unsigned char *pU, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_is_zero_n_u - true if p[0..n) is all zero octets.
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 -> 0)
 * n: number of octets to scan (0 -> 1, empty window is vacuously zero)
 *
 * Does not call libc. Early-exits on the first non-zero byte.
 */
int
gj_mem_is_zero_n_u(const void *p, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return 1;
	}
	if (p == NULL) {
		return 0;
	}
	return b2749_is_zero((const unsigned char *)p, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_is_zero_n_u(const void *p, size_t n)
    __attribute__((alias("gj_mem_is_zero_n_u")));
