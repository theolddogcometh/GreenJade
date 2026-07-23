/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1158: Ogg page capture-pattern magic check.
 *
 * Surface (unique symbols):
 *   int gj_ogg_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "OggS" and len >= 4;
 *       else 0 (FAIL). NULL / short → 0.
 *   int __gj_ogg_is_magic  (alias)
 *   __libcgj_batch1158_marker = "libcgj-batch1158"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1158_marker[] = "libcgj-batch1158";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ogg_is_magic — 1 if capture pattern is "OggS".
 */
int
gj_ogg_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'O' || p[1] != (unsigned char)'g' ||
	    p[2] != (unsigned char)'g' || p[3] != (unsigned char)'S') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ogg_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_ogg_is_magic")));
