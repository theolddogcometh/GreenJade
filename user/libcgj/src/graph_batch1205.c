/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1205: normalize newlines (CRLF/CR → LF) into out.
 *
 * Surface (unique symbols):
 *   int gj_str_normalize_newlines(const char *in, char *out, size_t cap);
 *     — Copy NUL-terminated in into out (capacity cap including NUL),
 *       converting "\r\n" and lone "\r" to '\n'. Lone '\n' is copied
 *       unchanged. Returns 0 on success; -1 on NULL args, cap == 0, or
 *       result (with NUL) does not fit. On failure out is undefined.
 *   int __gj_str_normalize_newlines  (alias)
 *   __libcgj_batch1205_marker = "libcgj-batch1205"
 *
 * Distinct from gj_str_strip_cr — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1205_marker[] = "libcgj-batch1205";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_normalize_newlines — CRLF and lone CR become LF into out.
 */
int
gj_str_normalize_newlines(const char *szIn, char *szOut, size_t cbCap)
{
	const char *pSrc;
	size_t iOut;

	if (szIn == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}

	pSrc = szIn;
	iOut = 0u;

	while (*pSrc != '\0') {
		char ch;

		ch = *pSrc;
		if (ch == '\r') {
			/* CRLF or lone CR → single LF. */
			if (pSrc[1] == '\n') {
				pSrc++;
			}
			ch = '\n';
		}
		if (iOut + 1u >= cbCap) {
			return -1;
		}
		szOut[iOut] = ch;
		iOut++;
		pSrc++;
	}

	if (iOut >= cbCap) {
		return -1;
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_normalize_newlines(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_str_normalize_newlines")));
