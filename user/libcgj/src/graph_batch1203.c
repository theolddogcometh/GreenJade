/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1203: collapse whitespace runs into one space.
 *
 * Surface (unique symbols):
 *   int gj_str_collapse_ws(const char *in, char *out, size_t cap);
 *     — Copy NUL-terminated in into out (capacity cap, including NUL),
 *       collapsing any run of ASCII whitespace (space/tab/CR/LF/VT/FF)
 *       to a single ' '. Leading/trailing whitespace is preserved as a
 *       single space if present (runs still collapse). Returns 0 on
 *       success; -1 on NULL args, cap == 0, or result (with NUL) does
 *       not fit in cap. On failure out is undefined.
 *   int __gj_str_collapse_ws  (alias)
 *   __libcgj_batch1203_marker = "libcgj-batch1203"
 *
 * Unique public name — avoid multi-def with trim/slugify helpers.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1203_marker[] = "libcgj-batch1203";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1203_is_ws(unsigned char ch)
{
	return ch == (unsigned char)' ' || ch == (unsigned char)'\t' ||
	    ch == (unsigned char)'\n' || ch == (unsigned char)'\r' ||
	    ch == (unsigned char)'\v' || ch == (unsigned char)'\f';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_collapse_ws — collapse whitespace runs to a single space into out.
 */
int
gj_str_collapse_ws(const char *szIn, char *szOut, size_t cbCap)
{
	const unsigned char *pSrc;
	size_t iOut;
	int fInWs;

	if (szIn == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}

	pSrc = (const unsigned char *)szIn;
	iOut = 0u;
	fInWs = 0;

	while (*pSrc != (unsigned char)'\0') {
		unsigned char ch;

		ch = *pSrc;
		pSrc++;
		if (b1203_is_ws(ch)) {
			if (fInWs == 0) {
				if (iOut + 1u >= cbCap) {
					return -1;
				}
				szOut[iOut] = ' ';
				iOut++;
				fInWs = 1;
			}
		} else {
			if (iOut + 1u >= cbCap) {
				return -1;
			}
			szOut[iOut] = (char)ch;
			iOut++;
			fInWs = 0;
		}
	}

	if (iOut >= cbCap) {
		return -1;
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_collapse_ws(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_str_collapse_ws")));
