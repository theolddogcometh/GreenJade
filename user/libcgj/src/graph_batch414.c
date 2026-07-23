/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch414: freestanding bounded C-string equality.
 *
 * Surface (unique symbols):
 *   int gj_str_eq_n(const char *a, const char *b, size_t n);
 *     — Return 1 if the first n octets of a and b match (or until both
 *       hit NUL within that window), else 0. Both NULL → 1 when n may
 *       be any; exactly one NULL → 0 when n > 0; n == 0 → 1.
 *   int __gj_str_eq_n  (alias)
 *   __libcgj_batch414_marker = "libcgj-batch414"
 *
 * Distinct from libc strncmp (ordering). This surface is boolean
 * equality over at most n octets; unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch414_marker[] = "libcgj-batch414";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_n — 1 if a and b match for up to n octets as C-string prefix.
 *
 * Compares min(n, length+1) style: stops at first mismatch or after n
 * octets. If a NUL is reached in both within the window at the same
 * index, they are equal for the remaining (shorter) content.
 *
 * Edge cases:
 *   n == 0 → 1 (vacuous)
 *   a == NULL && b == NULL → 1
 *   exactly one NULL and n > 0 → 0
 *
 * Does not call libc strncmp.
 */
int
gj_str_eq_n(const char *a, const char *b, size_t n)
{
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (a == NULL && b == NULL) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}

	for (i = 0u; i < n; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == '\0') {
			return 1;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_eq_n")));
