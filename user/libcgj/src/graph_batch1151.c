/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1151: JPEG SOI magic check.
 *
 * Surface (unique symbols):
 *   int gj_jpeg_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with SOI 0xFF 0xD8 and len >= 2;
 *       else 0 (FAIL). NULL / short → 0.
 *   int __gj_jpeg_is_magic  (alias)
 *   __libcgj_batch1151_marker = "libcgj-batch1151"
 *
 * Distinct from gj_jpeg_is_jpeg (batch110) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1151_marker[] = "libcgj-batch1151";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jpeg_is_magic — 1 if SOI marker FF D8 is present.
 */
int
gj_jpeg_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0xffu || p[1] != 0xd8u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_jpeg_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_jpeg_is_magic")));
