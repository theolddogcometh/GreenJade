/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1393: left zero-pad a string to a field width (zfill).
 *
 * Surface (unique symbols):
 *   int gj_str_zfill(const char *s, size_t width, char *out, size_t cap);
 *     — Left-pad s with '0' so the result has at least width characters.
 *       If s begins with '+' or '-', zeros are inserted after the sign
 *       (Python-style zfill). If strlen(s) >= width, copy s as-is.
 *       Returns length excl NUL, or -1 on error (out == NULL, cap == 0,
 *       s == NULL, or the result does not fit including NUL).
 *   int __gj_str_zfill  (alias)
 *   __libcgj_batch1393_marker = "libcgj-batch1393"
 *
 * Distinct from gj_str_pad_left (batch1082) — unique name; sign-aware.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1393_marker[] = "libcgj-batch1393";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1393_strlen(const char *sz)
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
 * gj_str_zfill — left zero-pad sz to at least cbWidth (sign-aware).
 */
int
gj_str_zfill(const char *sz, size_t cbWidth, char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbField;
	size_t cbZeros;
	size_t iSign;
	size_t i;
	size_t iOut;
	char chSign;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	cbSrc = b1393_strlen(sz);
	cbField = cbSrc;
	if (cbWidth > cbField) {
		cbField = cbWidth;
	}
	if (cbCap < cbField + 1u) {
		return -1;
	}

	iSign = 0u;
	chSign = '\0';
	if (cbSrc > 0u && (sz[0] == '+' || sz[0] == '-')) {
		iSign = 1u;
		chSign = sz[0];
	}

	cbZeros = cbField - cbSrc;
	iOut = 0u;
	if (iSign != 0u) {
		pOut[iOut++] = chSign;
	}
	for (i = 0u; i < cbZeros; i++) {
		pOut[iOut++] = '0';
	}
	for (i = iSign; i < cbSrc; i++) {
		pOut[iOut++] = sz[i];
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_zfill(const char *sz, size_t cbWidth, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_zfill")));
