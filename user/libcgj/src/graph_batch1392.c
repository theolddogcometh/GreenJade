/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1392: center a string with a pad character into cap.
 *
 * Surface (unique symbols):
 *   int gj_str_center_pad(const char *s, char pad, char *out, size_t cap);
 *     — Center s in a field of width (cap - 1) using pad on both sides
 *       (extra pad goes on the right when the remainder is odd). If
 *       strlen(s) >= field width, copy a truncated prefix of s that fits.
 *       Returns length excl NUL, or -1 on error (out == NULL, cap == 0,
 *       or s == NULL).
 *       Distinct from gj_str_center (batch1084) which takes an explicit
 *       width and fails when the full field does not fit.
 *   int __gj_str_center_pad  (alias)
 *   __libcgj_batch1392_marker = "libcgj-batch1392"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1392_marker[] = "libcgj-batch1392";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1392_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_center_pad — center sz with chPad into a field of size cap-1.
 *
 * Extra single pad (odd remainder) is placed on the right.
 */
int
gj_str_center_pad(const char *sz, char chPad, char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbField;
	size_t cbCopy;
	size_t cbPad;
	size_t cbLeft;
	size_t cbRight;
	size_t i;
	size_t iOut;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	cbField = cbCap - 1u;
	cbSrc = b1392_strlen(sz);
	cbCopy = cbSrc;
	if (cbCopy > cbField) {
		cbCopy = cbField;
	}

	if (cbField == 0u) {
		pOut[0] = '\0';
		return 0;
	}

	cbPad = cbField - cbCopy;
	cbLeft = cbPad / 2u;
	cbRight = cbPad - cbLeft;

	iOut = 0u;
	for (i = 0u; i < cbLeft; i++) {
		pOut[iOut++] = chPad;
	}
	for (i = 0u; i < cbCopy; i++) {
		pOut[iOut++] = sz[i];
	}
	for (i = 0u; i < cbRight; i++) {
		pOut[iOut++] = chPad;
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_center_pad(const char *sz, char chPad, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_center_pad")));
