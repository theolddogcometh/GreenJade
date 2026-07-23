/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1154: ZIP local-file header magic check.
 *
 * Surface (unique symbols):
 *   int gj_zip_is_magic(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with local file header signature
 *       PK\x03\x04 (0x50 0x4B 0x03 0x04) and len >= 4; else 0 (FAIL).
 *   int __gj_zip_is_magic  (alias)
 *   __libcgj_batch1154_marker = "libcgj-batch1154"
 *
 * Distinct from gj_zip_eocd_find (batch123) — avoid multi-def.
 * Does not accept EOCD-only or spanning signatures.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1154_marker[] = "libcgj-batch1154";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_is_magic — 1 if local-file signature PK\x03\x04.
 */
int
gj_zip_is_magic(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0x50u || p[1] != 0x4bu || p[2] != 0x03u ||
	    p[3] != 0x04u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_zip_is_magic(const void *pBuf, size_t cb)
    __attribute__((alias("gj_zip_is_magic")));
