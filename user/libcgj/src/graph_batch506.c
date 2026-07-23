/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch506: freestanding ASCII case-insensitive equality.
 *
 * Surface (unique symbols):
 *   int gj_str_eq_ci(const char *a, const char *b);
 *     — Return 1 if a and b are equal as C strings under ASCII
 *       case-insensitive compare (A-Z folds to a-z only), else 0.
 *       Both NULL → 1; exactly one NULL → 0.
 *   int __gj_str_eq_ci  (alias)
 *   __libcgj_batch506_marker = "libcgj-batch506"
 *
 * Distinct from libc strcasecmp (ordering / signed diff). This surface
 * is boolean equality only; unique gj_ names only. Does not call
 * strcasecmp / tolower.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch506_marker[] = "libcgj-batch506";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Fold A-Z to a-z; leave all other octets unchanged (ASCII only).
 */
static int
b506_fold_ascii(int nCh)
{
	if (nCh >= 'A' && nCh <= 'Z') {
		return nCh - 'A' + 'a';
	}
	return nCh;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq_ci — 1 if a equals b as C strings, ASCII case-insensitive.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 1
 *   exactly one of a, b is NULL → 0
 *   empty strings equal
 *
 * Does not call libc strcasecmp.
 */
int
gj_str_eq_ci(const char *a, const char *b)
{
	size_t i;
	int nCa;
	int nCb;

	if (a == NULL && b == NULL) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		nCa = b506_fold_ascii((unsigned char)a[i]);
		nCb = b506_fold_ascii((unsigned char)b[i]);
		if (nCa != nCb) {
			return 0;
		}
		if (nCa == 0) {
			return 1;
		}
		i++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_ci(const char *a, const char *b)
    __attribute__((alias("gj_str_eq_ci")));
