/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1157: PDF header magic check.
 *
 * Surface (unique symbols):
 *   int gj_pdf_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "%PDF" and len >= 4;
 *       else 0 (FAIL). Does not require version digits after "%PDF".
 *   int __gj_pdf_is_magic  (alias)
 *   __libcgj_batch1157_marker = "libcgj-batch1157"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1157_marker[] = "libcgj-batch1157";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pdf_is_magic — 1 if first four octets are "%PDF".
 */
int
gj_pdf_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != (unsigned char)'%' || p[1] != (unsigned char)'P' ||
	    p[2] != (unsigned char)'D' || p[3] != (unsigned char)'F') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pdf_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_pdf_is_magic")));
