/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1082: left-pad a string to a minimum field width
 *
 * Surface (unique symbols):
 *   int gj_str_pad_left(const char *s, size_t width, char pad,
 *                       char *out, size_t cap);
 *     — Copy s into out, left-padded with pad to at least width
 *       characters (if strlen(s) >= width, copy s as-is). NUL-
 *       terminated. Returns length excluding NUL, or -1 on error.
 *   int __gj_str_pad_left  (alias)

 *   __libcgj_batch1082_marker = "libcgj-batch1082"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1082_marker[] = "libcgj-batch1082";

static size_t
b1082_strlen(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * gj_str_pad_left — left-pad sz with chPad to at least cbWidth.
 */
int
gj_str_pad_left(const char *sz, size_t cbWidth, char chPad,
		char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbField;
	size_t cbPad;
	size_t i;
	size_t iOut;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	cbSrc = b1082_strlen(sz);
	cbField = cbSrc;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	if (cbCap < cbField + 1u) {
		return -1;
	}

	cbPad = cbField - cbSrc;
	iOut = 0u;
	for (i = 0u; i < cbPad; i++) {
		pOut[iOut++] = chPad;
	}
	for (i = 0u; i < cbSrc; i++) {
		pOut[iOut++] = sz[i];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

int __gj_str_pad_left(const char *sz, size_t cbWidth, char chPad,
		      char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_pad_left")));
