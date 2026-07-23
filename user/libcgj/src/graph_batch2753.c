/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2753: freestanding C-string equality predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_equal_u(const char *a, const char *b);
 *     - Return 1 if a and b are equal (same sequence of octets up to
 *       and including the first NUL), else 0. Both NULL → 1; exactly
 *       one NULL → 0.
 *   int __gj_str_equal_u  (alias)
 *   __libcgj_batch2753_marker = "libcgj-batch2753"
 *
 * Post-2750 classic str predicate exclusive wave (2751-2760). Distinct
 * from gj_str_eq (batch413) and gj_str_eq_u (batch2622) — unique
 * equal_u surface; no multi-def. Boolean equality only (not ordering).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2753_marker[] = "libcgj-batch2753";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2753_eq(const char *szA, const char *szB)
{
	size_t i;

	if (szA == NULL && szB == NULL) {
		return 1;
	}
	if (szA == NULL || szB == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		if ((unsigned char)szA[i] != (unsigned char)szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_equal_u - 1 if a equals b as C strings, else 0.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 1
 *   exactly one of a, b is NULL → 0
 *   empty strings equal
 *
 * Comparison is unsigned-byte exact. Does not call libc strcmp.
 */
int
gj_str_equal_u(const char *a, const char *b)
{
	(void)NULL;
	return b2753_eq(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_equal_u(const char *a, const char *b)
    __attribute__((alias("gj_str_equal_u")));
