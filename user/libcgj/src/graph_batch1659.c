/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1659: XZ stream header magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_xz_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with XZ stream header magic
 *       FD 37 7A 58 5A 00 ("\xFD7zXZ\0") and len >= 6; else 0 (FAIL).
 *       NULL / short → 0.
 *   int __gj_magic_is_xz_bytes  (alias)
 *   __libcgj_batch1659_marker = "libcgj-batch1659"
 *
 * Exclusive file-magic wave (1651–1659). Six-octet header only (no
 * stream flags / CRC check).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1659_marker[] = "libcgj-batch1659";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..5] is XZ header magic. Caller guarantees. */
static int
b1659_match_xz(const unsigned char *p)
{
	return (p[0] == 0xfdu && p[1] == 0x37u && p[2] == 0x7au &&
	        p[3] == 0x58u && p[4] == 0x5au && p[5] == 0x00u)
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_xz_bytes — 1 if first six octets are FD 37 7A 58 5A 00.
 */
int
gj_magic_is_xz_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 6u) {
		return 0;
	}
	return b1659_match_xz((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_xz_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_xz_bytes")));
