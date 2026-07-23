/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1165: TrueType sfnt version magic check.
 *
 * Surface (unique symbols):
 *   int gj_font_is_ttf(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with TrueType sfnt version
 *       0x00 0x01 0x00 0x00 or the tag "true", with len >= 4;
 *       else 0 (FAIL). Does not accept "OTTO" (see batch1166).
 *   int __gj_font_is_ttf  (alias)
 *   __libcgj_batch1165_marker = "libcgj-batch1165"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1165_marker[] = "libcgj-batch1165";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_is_ttf — 1 if sfnt is 00010000 or "true".
 */
int
gj_font_is_ttf(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	int fVer;
	int fTrue;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	fVer = (p[0] == 0x00u && p[1] == 0x01u && p[2] == 0x00u &&
	        p[3] == 0x00u);
	fTrue = (p[0] == (unsigned char)'t' && p[1] == (unsigned char)'r' &&
	         p[2] == (unsigned char)'u' && p[3] == (unsigned char)'e');
	if (!fVer && !fTrue) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_font_is_ttf(const void *pBuf, size_t cb)
    __attribute__((alias("gj_font_is_ttf")));
