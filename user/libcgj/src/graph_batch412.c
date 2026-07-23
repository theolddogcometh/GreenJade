/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch412: freestanding C-string copy.
 *
 * Surface (unique symbols):
 *   char *gj_str_cpy(char *d, const char *s);
 *     — Copy NUL-terminated s into d (including the terminator).
 *       Returns d. d == NULL or s == NULL → return d (no write).
 *   char *__gj_str_cpy  (alias)
 *   __libcgj_batch412_marker = "libcgj-batch412"
 *
 * Distinct from libc strcpy in string.c — unique gj_ names only.
 * Caller must ensure d has room for s including NUL.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch412_marker[] = "libcgj-batch412";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_cpy — copy s into d including the terminating NUL.
 *
 * d: destination buffer (must be large enough when both non-NULL)
 * s: NUL-terminated source string
 *
 * Does not call libc strcpy. Returns d. NULL d or s → return d unchanged.
 */
char *
gj_str_cpy(char *d, const char *s)
{
	size_t i;

	if (d == NULL || s == NULL) {
		return d;
	}

	i = 0u;
	for (;;) {
		d[i] = s[i];
		if (s[i] == '\0') {
			break;
		}
		i++;
	}
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_cpy(char *d, const char *s)
    __attribute__((alias("gj_str_cpy")));
