/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch863: reverse memory compare (end-first).
 *
 * Surface (unique symbols):
 *   int gj_mem_rcmp(const void *a, const void *b, size_t n);
 *     — Compare n octets from the end toward the start as unsigned
 *       chars. Returns 0 if equal; negative if a < b; positive if a > b
 *       at the last differing index (from the end). n==0 → 0.
 *       n>0 and either NULL → both NULL 0; a NULL -1; b NULL 1.
 *   int __gj_mem_rcmp  (alias)
 *   __libcgj_batch863_marker = "libcgj-batch863"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch863_marker[] = "libcgj-batch863";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_rcmp — reverse memcmp: last octet first, first octet last.
 *
 * Useful for little-endian multi-byte tie-breaks and suffix-oriented
 * ordering. Does not call libc memcmp.
 */
int
gj_mem_rcmp(const void *pA, const void *pB, size_t cN)
{
	const unsigned char *pUa;
	const unsigned char *pUb;
	size_t i;

	if (cN == 0u) {
		return 0;
	}
	if (pA == NULL || pB == NULL) {
		if (pA == pB) {
			return 0;
		}
		return (pA == NULL) ? -1 : 1;
	}
	if (pA == pB) {
		return 0;
	}

	pUa = (const unsigned char *)pA;
	pUb = (const unsigned char *)pB;
	i = cN;
	while (i > 0u) {
		i--;
		if (pUa[i] != pUb[i]) {
			return (int)pUa[i] - (int)pUb[i];
		}
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mem_rcmp(const void *pA, const void *pB, size_t cN)
    __attribute__((alias("gj_mem_rcmp")));
