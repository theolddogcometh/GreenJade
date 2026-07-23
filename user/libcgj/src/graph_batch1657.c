/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1657: gzip member magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_gzip_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with gzip ID1/ID2 0x1F 0x8B
 *       and len >= 2; else 0 (FAIL). NULL / short → 0.
 *   int __gj_magic_is_gzip_bytes  (alias)
 *   __libcgj_batch1657_marker = "libcgj-batch1657"
 *
 * Distinct from gj_gzip_is_magic (batch1148) / batch76 inflate helpers —
 * avoid multi-def. Exclusive file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1657_marker[] = "libcgj-batch1657";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..1] is gzip ID1/ID2. Caller guarantees. */
static int
b1657_match_gzip(const unsigned char *p)
{
	return (p[0] == 0x1fu && p[1] == 0x8bu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_gzip_bytes — 1 if ID1/ID2 are 1f 8b.
 */
int
gj_magic_is_gzip_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	return b1657_match_gzip((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_gzip_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_gzip_bytes")));
