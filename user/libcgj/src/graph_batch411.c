/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch411: freestanding C-string length.
 *
 * Surface (unique symbols):
 *   size_t gj_str_len(const char *s);
 *     — Count octets in s before the first NUL. s == NULL → 0.
 *   size_t __gj_str_len  (alias)
 *   __libcgj_batch411_marker = "libcgj-batch411"
 *
 * Distinct from libc strlen in string.c — unique gj_ names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch411_marker[] = "libcgj-batch411";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_len — length of a NUL-terminated string (not counting the NUL).
 *
 * s: NUL-terminated string, or NULL
 *
 * Does not call libc strlen. Returns 0 when s is NULL.
 */
size_t
gj_str_len(const char *s)
{
	size_t n;

	if (s == NULL) {
		return 0u;
	}

	n = 0u;
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_len(const char *s)
    __attribute__((alias("gj_str_len")));
