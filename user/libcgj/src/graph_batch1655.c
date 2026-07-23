/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1655: PDF header magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_pdf_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with "%PDF" and len >= 4;
 *       else 0 (FAIL). Does not require version digits after "%PDF".
 *   int __gj_magic_is_pdf_bytes  (alias)
 *   __libcgj_batch1655_marker = "libcgj-batch1655"
 *
 * Distinct from gj_pdf_is_magic (batch1157) — avoid multi-def.
 * Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1655_marker[] = "libcgj-batch1655";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is "%PDF". Caller guarantees p and length. */
static int
b1655_match_pdf(const unsigned char *p)
{
	return (p[0] == (unsigned char)'%' && p[1] == (unsigned char)'P' &&
	        p[2] == (unsigned char)'D' && p[3] == (unsigned char)'F')
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_pdf_bytes — 1 if first four octets are "%PDF".
 */
int
gj_magic_is_pdf_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1655_match_pdf((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_pdf_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_pdf_bytes")));
