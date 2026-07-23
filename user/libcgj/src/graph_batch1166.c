/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1166: OpenType CFF sfnt magic check.
 *
 * Surface (unique symbols):
 *   int gj_font_is_otf(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with OpenType CFF tag "OTTO"
 *       and len >= 4; else 0 (FAIL).
 *   int __gj_font_is_otf  (alias)
 *   __libcgj_batch1166_marker = "libcgj-batch1166"
 *
 * Distinct from gj_font_is_ttf (batch1165) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1166_marker[] = "libcgj-batch1166";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_is_otf — 1 if sfnt tag is "OTTO".
 */
int
gj_font_is_otf(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'O' || p[1] != (unsigned char)'T' ||
	    p[2] != (unsigned char)'T' || p[3] != (unsigned char)'O') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_font_is_otf(const void *pBuf, size_t cb)
    __attribute__((alias("gj_font_is_otf")));
