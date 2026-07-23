/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1086: hard-wrap a string at a fixed column width
 *
 * Surface (unique symbols):
 *   int gj_str_wrap_hard(const char *s, size_t width, char *out,
 *                        size_t cap);
 *     — Copy s into out inserting a newline after every width
 *       non-newline characters (hard wrap). Existing newlines reset
 *       the column. Does not break mid-codepoint (byte-oriented).
 *       Returns length excl NUL, or -1 on error / does not fit.
 *       width == 0 → -1.
 *   int __gj_str_wrap_hard  (alias)

 *   __libcgj_batch1086_marker = "libcgj-batch1086"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1086_marker[] = "libcgj-batch1086";

/*
 * gj_str_wrap_hard — insert newlines every cbWidth columns.
 *
 * Two-pass: first compute need, then write (or single-pass with cap check).
 */
int
gj_str_wrap_hard(const char *sz, size_t cbWidth, char *pOut, size_t cbCap)
{
	size_t i;
	size_t iOut;
	size_t cCol;
	char ch;

	if (pOut == NULL || cbCap == 0u || sz == NULL || cbWidth == 0u) {
		return -1;
	}

	iOut = 0u;
	cCol = 0u;
	for (i = 0u; sz[i] != '\0'; i++) {
		ch = sz[i];
		if (ch == '\n') {
			if (iOut + 1u >= cbCap) {
				return -1;
			}
			pOut[iOut++] = '\n';
			cCol = 0u;
			continue;
		}
		/* Need a hard break before this char if column full. */
		if (cCol == cbWidth) {
			if (iOut + 1u >= cbCap) {
				return -1;
			}
			pOut[iOut++] = '\n';
			cCol = 0u;
		}
		if (iOut + 1u >= cbCap) {
			return -1;
		}
		pOut[iOut++] = ch;
		cCol++;
	}
	if (iOut >= cbCap) {
		return -1;
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

int __gj_str_wrap_hard(const char *sz, size_t cbWidth, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_wrap_hard")));
