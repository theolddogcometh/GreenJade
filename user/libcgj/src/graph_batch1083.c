/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1083: right-pad a string to a minimum field width
 *
 * Surface (unique symbols):
 *   int gj_str_pad_right(const char *s, size_t width, char pad,
 *                        char *out, size_t cap);
 *     — Copy s into out, right-padded with pad to at least width
 *       characters (if strlen(s) >= width, copy s as-is). NUL-
 *       terminated. Returns length excluding NUL, or -1 on error.
 *   int __gj_str_pad_right  (alias)

 *   __libcgj_batch1083_marker = "libcgj-batch1083"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1083_marker[] = "libcgj-batch1083";

static size_t
b1083_strlen(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * gj_str_pad_right — right-pad sz with chPad to at least cbWidth.
 */
int
gj_str_pad_right(const char *sz, size_t cbWidth, char chPad,
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

	cbSrc = b1083_strlen(sz);
	cbField = cbSrc;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	if (cbCap < cbField + 1u) {
		return -1;
	}

	iOut = 0u;
	for (i = 0u; i < cbSrc; i++) {
		pOut[iOut++] = sz[i];
	}
	cbPad = cbField - cbSrc;
	for (i = 0u; i < cbPad; i++) {
		pOut[iOut++] = chPad;
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

int __gj_str_pad_right(const char *sz, size_t cbWidth, char chPad,
		       char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_pad_right")));
