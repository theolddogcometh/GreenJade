/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch413: freestanding C-string equality.
 *
 * Surface (unique symbols):
 *   int gj_str_eq(const char *a, const char *b);
 *     — Return 1 if a and b are equal (same sequence of octets up to
 *       and including the first NUL), else 0. Both NULL → 1; exactly
 *       one NULL → 0.
 *   int __gj_str_eq  (alias)
 *   __libcgj_batch413_marker = "libcgj-batch413"
 *
 * Distinct from libc strcmp (ordering / signed diff). This surface is
 * boolean equality only; unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch413_marker[] = "libcgj-batch413";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_eq — 1 if a equals b as C strings, else 0.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 1
 *   exactly one of a, b is NULL → 0
 *   empty strings equal
 *
 * Does not call libc strcmp.
 */
int
gj_str_eq(const char *a, const char *b)
{
	size_t i;

	if (a == NULL && b == NULL) {
		return 1;
	}
	if (a == NULL || b == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		if (a[i] != b[i]) {
			return 0;
		}
		if (a[i] == '\0') {
			return 1;
		}
		i++;
	}
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq(const char *a, const char *b)
    __attribute__((alias("gj_str_eq")));
