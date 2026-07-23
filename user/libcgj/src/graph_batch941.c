/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch941: count lines in a C string.
 *
 * Surface (unique symbols):
 *   size_t gj_str_count_lines(const char *s);
 *     — Number of lines in s. Empty/NULL → 0. Each run of non-NUL
 *       chars ending at '\n' or EOS counts as one line (trailing
 *       newline does not add an extra empty line).
 *   size_t __gj_str_count_lines  (alias)
 *   __libcgj_batch941_marker = "libcgj-batch941"

 *
 * Does NOT redefine gj_str_len (batch411) — line count only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch941_marker[] = "libcgj-batch941";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_count_lines — count logical lines in a NUL-terminated string.
 *
 * A line is a maximal run of characters that does not contain '\n',
 * terminated either by '\n' or by end-of-string. An empty string has
 * zero lines; a lone "\n" has one (empty) line.
 */
size_t
gj_str_count_lines(const char *pS)
{
	size_t cLines;
	const char *p;

	if (pS == NULL || pS[0] == '\0') {
		return 0u;
	}

	cLines = 0u;
	p = pS;
	while (*p != '\0') {
		cLines++;
		while (*p != '\0' && *p != '\n') {
			p++;
		}
		if (*p == '\n') {
			p++;
		}
	}
	return cLines;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_count_lines(const char *pS)
    __attribute__((alias("gj_str_count_lines")));
