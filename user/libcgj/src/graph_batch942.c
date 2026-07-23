/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch942: pointer to start of line L (0-based).
 *
 * Surface (unique symbols):
 *   const char *gj_str_line_at(const char *s, size_t line);
 *     — Return pointer to the first char of 0-based line L in s.
 *       NULL s, or L past last line → NULL.
 *   const char *__gj_str_line_at  (alias)
 *   __libcgj_batch942_marker = "libcgj-batch942"

 *
 * Does NOT redefine gj_str_count_lines (batch941).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch942_marker[] = "libcgj-batch942";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_line_at — locate the start of line L (0-based) in s.
 *
 * Lines match gj_str_count_lines semantics: each run up to '\n' or EOS.
 * Returns a pointer into s, or NULL when L is out of range / s is NULL.
 */
const char *
gj_str_line_at(const char *pS, size_t cLine)
{
	size_t cCur;
	const char *p;

	if (pS == NULL || pS[0] == '\0') {
		return NULL;
	}

	cCur = 0u;
	p = pS;
	while (*p != '\0') {
		if (cCur == cLine) {
			return p;
		}
		while (*p != '\0' && *p != '\n') {
			p++;
		}
		if (*p == '\n') {
			p++;
		}
		cCur++;
	}
	return NULL;
}

/* ---- underscored alias ------------------------------------------------- */

const char *__gj_str_line_at(const char *pS, size_t cLine)
    __attribute__((alias("gj_str_line_at")));
