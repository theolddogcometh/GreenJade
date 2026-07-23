/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1043: common prefix length of two C strings.
 *
 * Surface (unique symbols):
 *   size_t gj_str_common_prefix_len(const char *a, const char *b);
 *     — Byte length of the longest shared prefix of a and b. Comparison
 *       is unsigned-char ordinal (not locale). Stops at the first
 *       mismatch or either NUL. NULL a or NULL b → 0.
 *   size_t __gj_str_common_prefix_len  (alias)
 *   __libcgj_batch1043_marker = "libcgj-batch1043"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1043_marker[] = "libcgj-batch1043";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_common_prefix_len — shared prefix byte count of a and b.
 */
size_t
gj_str_common_prefix_len(const char *a, const char *b)
{
	size_t i;

	if (a == NULL || b == NULL) {
		return 0u;
	}

	i = 0u;
	while (a[i] != '\0' && b[i] != '\0' &&
	    (unsigned char)a[i] == (unsigned char)b[i]) {
		i++;
	}
	return i;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_common_prefix_len(const char *a, const char *b)
    __attribute__((alias("gj_str_common_prefix_len")));
