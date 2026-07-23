/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1656: ZIP local-file header magic (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_zip_local(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with local file header signature
 *       PK\x03\x04 (0x50 0x4B 0x03 0x04) and len >= 4; else 0 (FAIL).
 *   int __gj_magic_is_zip_local  (alias)
 *   __libcgj_batch1656_marker = "libcgj-batch1656"
 *
 * Distinct from gj_zip_is_magic / gj_zip_eocd_find — avoid multi-def.
 * Does not accept EOCD-only (PK\x05\x06) or spanning signatures.
 * Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1656_marker[] = "libcgj-batch1656";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is local-file signature. Caller guarantees. */
static int
b1656_match_local(const unsigned char *p)
{
	return (p[0] == 0x50u && p[1] == 0x4bu && p[2] == 0x03u &&
	        p[3] == 0x04u)
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_zip_local — 1 if local-file signature PK\x03\x04.
 */
int
gj_magic_is_zip_local(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1656_match_local((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_zip_local(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_zip_local")));
