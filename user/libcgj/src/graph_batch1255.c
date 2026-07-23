/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1255: constant-time (non-short-circuit) memcpy.
 *
 * Surface (unique symbols):
 *   void *gj_ct_memcpy(void *dst, const void *src, size_t n);
 *     — Copy n octets from src to dst. Always walks full length
 *       (no early exit). Regions must not overlap. Returns dst.
 *       NULL dst/src with n>0 → returns dst unchanged (no-op).
 *       n==0 → returns dst.
 *   void *__gj_ct_memcpy  (alias)
 *   __libcgj_batch1255_marker = "libcgj-batch1255"
 *
 * Distinct from gj_mem_cpy / memcpy — unique ct_ surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1255_marker[] = "libcgj-batch1255";

/*
 * gj_ct_memcpy — forward octet copy of fixed length n (always full walk).
 */
void *
gj_ct_memcpy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD;
	const unsigned char *pS;
	size_t i;

	if (cb == 0u || pDst == NULL || pSrc == NULL) {
		return pDst;
	}

	pD = (unsigned char *)pDst;
	pS = (const unsigned char *)pSrc;
	for (i = 0u; i < cb; i++) {
		pD[i] = pS[i];
	}
	return pDst;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_ct_memcpy(void *pDst, const void *pSrc, size_t cb)
    __attribute__((alias("gj_ct_memcpy")));
