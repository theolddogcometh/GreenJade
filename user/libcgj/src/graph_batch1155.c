/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1155: ustar tar magic at offset 257.
 *
 * Surface (unique symbols):
 *   int gj_tar_is_ustar(const void *buf, size_t len);
 *     — Return 1 (OK) if buf has at least 262 bytes and octets
 *       [257..261] are "ustar" (POSIX / GNU ustar magic); else 0.
 *       Does not require version field or full 512-byte block.
 *   int __gj_tar_is_ustar  (alias)
 *   __libcgj_batch1155_marker = "libcgj-batch1155"
 *
 * Distinct from gj_tar_header_parse / b101_is_ustar (batch101) public
 * surface — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1155_marker[] = "libcgj-batch1155";

#define B1155_USTAR_OFF  257u
#define B1155_USTAR_LEN  5u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tar_is_ustar — 1 if magic field at 257 is "ustar".
 */
int
gj_tar_is_ustar(const void *pBuf, size_t cb)
{
	const unsigned char *p;
	const unsigned char *pM;

	if (pBuf == NULL || cb < (B1155_USTAR_OFF + B1155_USTAR_LEN)) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	pM = p + B1155_USTAR_OFF;
	if (pM[0] != (unsigned char)'u' || pM[1] != (unsigned char)'s' ||
	    pM[2] != (unsigned char)'t' || pM[3] != (unsigned char)'a' ||
	    pM[4] != (unsigned char)'r') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tar_is_ustar(const void *pBuf, size_t cb)
    __attribute__((alias("gj_tar_is_ustar")));
