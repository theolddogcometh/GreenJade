/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1167: WOFF / WOFF2 container magic check.
 *
 * Surface (unique symbols):
 *   int gj_font_is_woff(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "wOFF" or "wOF2" and
 *       len >= 4; else 0 (FAIL).
 *   int __gj_font_is_woff  (alias)
 *   __libcgj_batch1167_marker = "libcgj-batch1167"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1167_marker[] = "libcgj-batch1167";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_is_woff — 1 if signature is wOFF or wOF2.
 */
int
gj_font_is_woff(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	int fWoff;
	int fWof2;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'w' || p[1] != (unsigned char)'O' ||
	    p[2] != (unsigned char)'F') {
		return 0;
	}
	fWoff = (p[3] == (unsigned char)'F');
	fWof2 = (p[3] == (unsigned char)'2');
	if (!fWoff && !fWof2) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_font_is_woff(const void *pBuf, size_t cb)
    __attribute__((alias("gj_font_is_woff")));
