/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1084: center a string in a fixed field width
 *
 * Surface (unique symbols):
 *   int gj_str_center(const char *s, size_t width, char pad,
 *                     char *out, size_t cap);
 *     — Center s in a field of width characters using pad on both
 *       sides (extra pad goes on the right when width - len is odd).
 *       If strlen(s) >= width, copy s as-is. Returns length excl NUL
 *       or -1 on error.
 *   int __gj_str_center  (alias)

 *   __libcgj_batch1084_marker = "libcgj-batch1084"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1084_marker[] = "libcgj-batch1084";

static size_t
b1084_strlen(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * gj_str_center — center sz in a field of cbWidth using chPad.
 *
 * Extra single pad (odd remainder) is placed on the right.
 */
int
gj_str_center(const char *sz, size_t cbWidth, char chPad,
	      char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbField;
	size_t cbPad;
	size_t cbLeft;
	size_t cbRight;
	size_t i;
	size_t iOut;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	cbSrc = b1084_strlen(sz);
	cbField = cbSrc;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	if (cbCap < cbField + 1u) {
		return -1;
	}

	cbPad = cbField - cbSrc;
	cbLeft = cbPad / 2u;
	cbRight = cbPad - cbLeft;

	iOut = 0u;
	for (i = 0u; i < cbLeft; i++) {
		pOut[iOut++] = chPad;
	}
	for (i = 0u; i < cbSrc; i++) {
		pOut[iOut++] = sz[i];
	}
	for (i = 0u; i < cbRight; i++) {
		pOut[iOut++] = chPad;
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

int __gj_str_center(const char *sz, size_t cbWidth, char chPad,
		    char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_center")));
