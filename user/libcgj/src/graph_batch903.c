/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch903: trim whitespace into a separate buffer.
 *
 * Surface (unique symbols):
 *   int gj_str_trim_copy(const char *src, char *out, size_t out_cap);
 *     — Copy src into out with leading and trailing space / tab / CR /
 *       LF removed. Always NUL-terminates out on success. Returns 0 on
 *       success, -1 on bad args or insufficient out_cap.
 *   int __gj_str_trim_copy  (alias)
 *   __libcgj_batch903_marker = "libcgj-batch903"
 *
 * Distinct from gj_str_trim (batch357, in-place) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch903_marker[] = "libcgj-batch903";

/* ---- helpers ----------------------------------------------------------- */

static int
b903_is_trim(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_copy — trim leading/trailing whitespace into out.
 *
 * src:     NUL-terminated source (NULL → -1)
 * out:     destination buffer (NULL → -1)
 * out_cap: capacity of out including the terminating NUL
 *
 * Empty / all-whitespace src yields an empty string in out (needs
 * out_cap >= 1). Returns 0 on success, -1 on failure (out unchanged
 * on capacity failure after measure).
 */
int
gj_str_trim_copy(const char *src, char *out, size_t out_cap)
{
	size_t iStart;
	size_t iEnd;
	size_t cb;
	size_t i;

	if (src == NULL || out == NULL || out_cap == 0u) {
		return -1;
	}

	/* Leading trim. */
	iStart = 0u;
	while (src[iStart] != '\0' && b903_is_trim(src[iStart])) {
		iStart++;
	}

	/* Empty after leading trim. */
	if (src[iStart] == '\0') {
		out[0] = '\0';
		return 0;
	}

	/* Find end (one past last char). */
	iEnd = iStart;
	while (src[iEnd] != '\0') {
		iEnd++;
	}

	/* Trailing trim: back up iEnd over trim characters. */
	while (iEnd > iStart && b903_is_trim(src[iEnd - 1u])) {
		iEnd--;
	}

	cb = iEnd - iStart;
	if (cb + 1u > out_cap) {
		return -1;
	}

	for (i = 0u; i < cb; i++) {
		out[i] = src[iStart + i];
	}
	out[cb] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_trim_copy(const char *src, char *out, size_t out_cap)
    __attribute__((alias("gj_str_trim_copy")));
