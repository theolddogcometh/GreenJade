/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1044: common suffix length of two C strings.
 *
 * Surface (unique symbols):
 *   size_t gj_str_common_suffix_len(const char *a, const char *b);
 *     — Byte length of the longest shared suffix of a and b. Comparison
 *       is unsigned-char ordinal. NULL a or NULL b → 0.
 *   size_t __gj_str_common_suffix_len  (alias)
 *   __libcgj_batch1044_marker = "libcgj-batch1044"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1044_marker[] = "libcgj-batch1044";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1044_len(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_common_suffix_len — shared suffix byte count of a and b.
 */
size_t
gj_str_common_suffix_len(const char *a, const char *b)
{
	size_t nA;
	size_t nB;
	size_t i;

	if (a == NULL || b == NULL) {
		return 0u;
	}

	nA = b1044_len(a);
	nB = b1044_len(b);
	i = 0u;
	while (i < nA && i < nB &&
	    (unsigned char)a[nA - 1u - i] ==
	        (unsigned char)b[nB - 1u - i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_common_suffix_len(const char *a, const char *b)
    __attribute__((alias("gj_str_common_suffix_len")));
