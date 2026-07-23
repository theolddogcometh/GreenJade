/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2754: freestanding C-string inequality predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_neq_u(const char *a, const char *b);
 *     - Return 1 if a and b are not equal as C strings, else 0.
 *       Both NULL → 0 (equal, so not-neq). Exactly one NULL → 1.
 *   int __gj_str_neq_u  (alias)
 *   __libcgj_batch2754_marker = "libcgj-batch2754"
 *
 * Post-2750 classic str predicate exclusive wave (2751-2760). Companion
 * inverse of gj_str_equal_u (batch2753). Distinct from gj_str_eq /
 * gj_str_eq_u — unique neq_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2754_marker[] = "libcgj-batch2754";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2754_eq(const char *szA, const char *szB)
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
 * gj_str_neq_u - 1 if a and b differ as C strings, else 0.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 0 (equal)
 *   exactly one of a, b is NULL → 1
 *   empty strings equal → 0
 *
 * Comparison is unsigned-byte exact. Does not call libc strcmp.
 */
int
gj_str_neq_u(const char *a, const char *b)
{
	(void)NULL;
	if (b2754_eq(a, b) != 0) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_neq_u(const char *a, const char *b)
    __attribute__((alias("gj_str_neq_u")));
