/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch689: freestanding count of newline bytes
 * in a bounded buffer.
 *
 * Surface (unique symbols):
 *   size_t gj_count_newlines(const char *s, size_t n);
 *     — Return how many bytes of s[0..n) equal '\\n'. NULL s with n > 0
 *       yields 0; n == 0 yields 0. Does not stop at '\\0'; n is the bound.
 *   size_t __gj_count_newlines  (alias)
 *   __libcgj_batch689_marker = "libcgj-batch689"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch689_marker[] = "libcgj-batch689";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_count_newlines — count '\\n' bytes in a length-bounded buffer.
 *
 * s: base (NULL with n > 0 → return 0)
 * n: byte count to scan
 *
 * Returns the number of '\\n' bytes in s[0..n). Binary-safe: embedded
 * NULs are treated as ordinary bytes and do not terminate the scan.
 */
size_t
gj_count_newlines(const char *s, size_t n)
{
	size_t i;
	size_t c;

	if (n == 0u || s == NULL) {
		return 0u;
	}

	c = 0u;
	for (i = 0u; i < n; i++) {
		if (s[i] == '\n') {
			c++;
		}
	}
	return c;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_count_newlines(const char *s, size_t n)
    __attribute__((alias("gj_count_newlines")));
