/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch449: freestanding all-decimal-digit string check.
 *
 * Surface (unique symbols):
 *   int gj_is_digit_str(const char *s);
 *     — Return 1 if s is a non-empty string of ASCII digits '0'..'9'
 *       only (full string). Return 0 for NULL, empty, or any non-digit.
 *   int __gj_is_digit_str  (alias)
 *   __libcgj_batch449_marker = "libcgj-batch449"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch449_marker[] = "libcgj-batch449";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_digit_str — 1 iff s is nonempty and every character is '0'..'9'.
 *
 * Edge cases:
 *   s == NULL → 0
 *   ""        → 0
 *   "0" / "42" / "00" → 1
 *   "+1" / "-1" / "1a" / " 1" → 0
 */
int
gj_is_digit_str(const char *s)
{
	const char *p;
	int nDig;

	if (s == NULL) {
		return 0;
	}

	p = s;
	nDig = 0;
	while (*p != '\0') {
		if (*p < '0' || *p > '9') {
			return 0;
		}
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_digit_str(const char *s)
    __attribute__((alias("gj_is_digit_str")));
