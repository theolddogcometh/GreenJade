/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1652: PE/DOS MZ magic bytes (lightweight).
 *
 * Surface (unique symbols):
 *   int gj_magic_is_pe_bytes(const void *buf, size_t len);
 *     — Return 1 (OK) if buf begins with DOS "MZ" (0x4D 0x5A) and
 *       len >= 2; else 0 (FAIL). NULL / short → 0.
 *   int __gj_magic_is_pe_bytes  (alias)
 *   __libcgj_batch1652_marker = "libcgj-batch1652"
 *
 * Distinct from gj_pe_is_mz / gj_pe_is_pe — avoid multi-def. Exclusive
 * file-magic wave (1651–1659). First-octet sniff only (no e_lfanew).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1652_marker[] = "libcgj-batch1652";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p[0..1] is IMAGE_DOS_HEADER.e_magic "MZ". Caller guarantees. */
static int
b1652_match_mz(const unsigned char *p)
{
	return (p[0] == (unsigned char)'M' && p[1] == (unsigned char)'Z')
	           ? 1
	           : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_magic_is_pe_bytes — 1 if first two octets are "MZ".
 */
int
gj_magic_is_pe_bytes(const void *pBuf, size_t cb)
{
	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	return b1652_match_mz((const unsigned char *)pBuf);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_magic_is_pe_bytes(const void *pBuf, size_t cb)
    __attribute__((alias("gj_magic_is_pe_bytes")));
