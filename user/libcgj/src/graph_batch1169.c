/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1169: DER ASN.1 SEQUENCE tag check.
 *
 * Surface (unique symbols):
 *   int gj_cert_is_der_seq(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with ASN.1 universal SEQUENCE
 *       tag 0x30 and len >= 1; else 0 (FAIL). Does not parse length.
 *   int __gj_cert_is_der_seq  (alias)
 *   __libcgj_batch1169_marker = "libcgj-batch1169"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1169_marker[] = "libcgj-batch1169";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cert_is_der_seq — 1 if first octet is ASN.1 SEQUENCE (0x30).
 */
int
gj_cert_is_der_seq(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 1u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x30u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cert_is_der_seq(const void *pBuf, size_t cb)
    __attribute__((alias("gj_cert_is_der_seq")));
