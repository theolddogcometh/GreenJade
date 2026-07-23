/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1658: Zstandard frame magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_zstd_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with Zstandard magic number
 *       0x28 0xB5 0x2F 0xFD (little-endian 0xFD2FB528) and len >= 4;
 *       else 0 (FAIL). NULL / short → 0.
 *   int __gj_magic_is_zstd_bytes  (alias)
 *   __libcgj_batch1658_marker = "libcgj-batch1658"
 *
 * Does not accept skippable-frame magics (0x184D2A5*). Exclusive
 * file-magic wave (1651–1659).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1658_marker[] = "libcgj-batch1658";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..3] is ZSTD_MAGICNUMBER octets. Caller guarantees. */
static int
b1658_match_zstd(const unsigned char *p)
{
	return (p[0] == 0x28u && p[1] == 0xb5u && p[2] == 0x2fu &&
	        p[3] == 0xfdu)
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_zstd_bytes — 1 if first four octets are 28 B5 2F FD.
 */
int
gj_magic_is_zstd_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 4u) {
		return 0;
	}
	return b1658_match_zstd((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_zstd_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_zstd_bytes")));
