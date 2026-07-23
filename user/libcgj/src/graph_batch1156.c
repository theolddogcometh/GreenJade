/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1156: SQLite database header magic check.
 *
 * Surface (unique symbols):
 *   int gj_sqlite_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with the 16-byte SQLite header
 *       "SQLite format 3\0" and len >= 16; else 0 (FAIL).
 *   int __gj_sqlite_is_magic  (alias)
 *   __libcgj_batch1156_marker = "libcgj-batch1156"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1156_marker[] = "libcgj-batch1156";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sqlite_is_magic — 1 if header is "SQLite format 3\0".
 */
int
gj_sqlite_is_magic(const void *pBuf, size_t cb)
{
	static const unsigned char aSig[16] = {
	    (unsigned char)'S', (unsigned char)'Q', (unsigned char)'L',
	    (unsigned char)'i', (unsigned char)'t', (unsigned char)'e',
	    (unsigned char)' ', (unsigned char)'f', (unsigned char)'o',
	    (unsigned char)'r', (unsigned char)'m', (unsigned char)'a',
	    (unsigned char)'t', (unsigned char)' ', (unsigned char)'3',
	    0x00u
	};
	const unsigned char *p;
	unsigned i;

	if (pBuf == NULL || cb < 16u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	for (i = 0u; i < 16u; i++) {
		if (p[i] != aSig[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sqlite_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_sqlite_is_magic")));
