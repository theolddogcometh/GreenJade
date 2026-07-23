/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2411: case-insensitive equality of n bytes.
 *
 * Surface (unique symbols):
 *   int gj_str_eq_ci_n(const char *a, const char *b, size_t n);
 *     - Return 1 if a and b match for n octets under ASCII case fold
 *       (A-Z -> a-z), else 0. Binary-safe (does not stop at NUL).
 *       n == 0 -> 1. Either NULL with n > 0 -> 0. Both NULL with n == 0
 *       -> 1.
 *   int __gj_str_eq_ci_n  (alias)
 *   __libcgj_batch2411_marker = "libcgj-batch2411"
 *
 * Distinct from gj_str_eq_ci (NUL strings, batch506), gj_str_eq_n
 * (case-sensitive, batch414), and gj_ascii_fold_eq_n (batch1629) —
 * unique gj_str_eq_ci_n surface only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2411_marker[] = "libcgj-batch2411";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b2411_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_ci_n - ASCII case-insensitive equality of n octets.
 *
 * a, b: regions of at least n bytes when n > 0
 * n:    number of octets to compare
 *
 * Folds A-Z to a-z on both sides, then compares. Does not stop at NUL.
 * Early-exits on the first folded mismatch.
 */
int
gj_str_eq_ci_n(const char *a, const char *b, size_t n)
{
	const unsigned char *pa;
	const unsigned char *pb;
	size_t i;

	if (n == 0u) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}
	if (a == b) {
		return 1;
	}

	pa = (const unsigned char *)a;
	pb = (const unsigned char *)b;
	for (i = 0u; i < n; i++) {
		if (b2411_fold(pa[i]) != b2411_fold(pb[i])) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_ci_n(const char *a, const char *b, size_t n)
    __attribute__((alias("gj_str_eq_ci_n")));
