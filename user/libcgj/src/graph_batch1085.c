/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1085: truncate a string with an ellipsis suffix
 *
 * Surface (unique symbols):
 *   int gj_str_truncate_ellipsis(const char *s, size_t max_len,
 *                                char *out, size_t cap);
 *     — If strlen(s) <= max_len, copy s. Else, if max_len >= 3, write
 *       the first (max_len - 3) characters of s followed by "...".
 *       If max_len < 3 and truncation is needed, write the first
 *       max_len characters (no ellipsis). Returns length excl NUL or -1.
 *   int __gj_str_truncate_ellipsis  (alias)

 *   __libcgj_batch1085_marker = "libcgj-batch1085"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1085_marker[] = "libcgj-batch1085";

static size_t
b1085_strlen(const char *sz)
{
	size_t n;

	n = 0u;
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * gj_str_truncate_ellipsis — copy/truncate sz into pOut with "..." if cut.
 */
int
gj_str_truncate_ellipsis(const char *sz, size_t cbMax,
			 char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbKeep;
	size_t i;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	cbSrc = b1085_strlen(sz);
	if (cbSrc <= cbMax) {
		if (cbCap < cbSrc + 1u) {
			return -1;
		}
		for (i = 0u; i < cbSrc; i++) {
			pOut[i] = sz[i];
		}
		pOut[cbSrc] = '\0';
		return (int)cbSrc;
	}

	/* Truncation path. */
	if (cbCap < cbMax + 1u) {
		return -1;
	}

	if (cbMax >= 3u) {
		cbKeep = cbMax - 3u;
		for (i = 0u; i < cbKeep; i++) {
			pOut[i] = sz[i];
		}
		pOut[cbKeep] = '.';
		pOut[cbKeep + 1u] = '.';
		pOut[cbKeep + 2u] = '.';
		pOut[cbMax] = '\0';
		return (int)cbMax;
	}

	/* max_len < 3: hard cut, no room for ellipsis. */
	for (i = 0u; i < cbMax; i++) {
		pOut[i] = sz[i];
	}
	pOut[cbMax] = '\0';
	return (int)cbMax;
}

int __gj_str_truncate_ellipsis(const char *sz, size_t cbMax,
			       char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_truncate_ellipsis")));
