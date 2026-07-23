/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1225: Z-algorithm single-position prefix length.
 *
 * Surface (unique symbols):
 *   size_t gj_z_algorithm_prefix(const char *s, size_t n, size_t i);
 *     — Return the length of the longest substring starting at i that
 *       matches a prefix of s[0..n) (the classic Z[i] value). For i==0
 *       returns 0 by Z-convention (not n). i >= n → 0. NULL s with n>0
 *       → 0. Explicit compare only (no full Z-box table); O(n) per call.
 *   size_t __gj_z_algorithm_prefix  (alias)
 *   __libcgj_batch1225_marker = "libcgj-batch1225"
 *
 * Distinct from gj_z_array (batch241) which fills a full Z table.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1225_marker[] = "libcgj-batch1225";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_z_algorithm_prefix — Z[i] for s[0..n) via direct prefix compare.
 */
size_t
gj_z_algorithm_prefix(const char *szS, size_t cN, size_t iPos)
{
	size_t cMatch;

	if (cN == 0u || iPos >= cN) {
		return 0u;
	}
	if (szS == NULL) {
		return 0u;
	}
	if (iPos == 0u) {
		return 0u;
	}

	cMatch = 0u;
	while (iPos + cMatch < cN &&
	       szS[cMatch] == szS[iPos + cMatch]) {
		cMatch++;
	}
	return cMatch;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_z_algorithm_prefix(const char *szS, size_t cN, size_t iPos)
    __attribute__((alias("gj_z_algorithm_prefix")));
