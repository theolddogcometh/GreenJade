/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1152: GIF signature magic check.
 *
 * Surface (unique symbols):
 *   int gj_gif_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "GIF87a" or "GIF89a" and
 *       len >= 6; else 0 (FAIL). NULL / short → 0.
 *   int __gj_gif_is_magic  (alias)
 *   __libcgj_batch1152_marker = "libcgj-batch1152"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1152_marker[] = "libcgj-batch1152";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gif_is_magic — 1 if signature is GIF87a or GIF89a.
 */
int
gj_gif_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 6u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'G' || p[1] != (unsigned char)'I' ||
	    p[2] != (unsigned char)'F' || p[3] != (unsigned char)'8') {
		return 0;
	}
	if (p[4] != (unsigned char)'7' && p[4] != (unsigned char)'9') {
		return 0;
	}
	if (p[5] != (unsigned char)'a') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gif_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_gif_is_magic")));
