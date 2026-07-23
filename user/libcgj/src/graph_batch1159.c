/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1159: FLAC stream marker magic check.
 *
 * Surface (unique symbols):
 *   int gj_flac_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "fLaC" and len >= 4;
 *       else 0 (FAIL). NULL / short → 0.
 *   int __gj_flac_is_magic  (alias)
 *   __libcgj_batch1159_marker = "libcgj-batch1159"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1159_marker[] = "libcgj-batch1159";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flac_is_magic — 1 if stream marker is "fLaC".
 */
int
gj_flac_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'f' || p[1] != (unsigned char)'L' ||
	    p[2] != (unsigned char)'a' || p[3] != (unsigned char)'C') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_flac_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_flac_is_magic")));
