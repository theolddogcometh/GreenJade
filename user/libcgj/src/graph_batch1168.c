/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1168: PEM armor BEGIN line prefix check.
 *
 * Surface (unique symbols):
 *   int gj_cert_is_pem_begin(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "-----BEGIN " (11 octets)
 *       and len >= 11; else 0 (FAIL). Does not require a full label.
 *   int __gj_cert_is_pem_begin  (alias)
 *   __libcgj_batch1168_marker = "libcgj-batch1168"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1168_marker[] = "libcgj-batch1168";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cert_is_pem_begin — 1 if prefix is "-----BEGIN ".
 */
int
gj_cert_is_pem_begin(const void *pBuf, size_t cb)
{
	static const unsigned char aSig[11] = {
	    (unsigned char)'-', (unsigned char)'-', (unsigned char)'-',
	    (unsigned char)'-', (unsigned char)'-', (unsigned char)'B',
	    (unsigned char)'E', (unsigned char)'G', (unsigned char)'I',
	    (unsigned char)'N', (unsigned char)' '
	};
	const unsigned char *p;
	unsigned i;

	if (pBuf == NULL || cb < 11u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	for (i = 0u; i < 11u; i++) {
		if (p[i] != aSig[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cert_is_pem_begin(const void *pBuf, size_t cb)
    __attribute__((alias("gj_cert_is_pem_begin")));
