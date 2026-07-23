/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1286: binary search over sorted C string table.
 *
 * Surface (unique symbols):
 *   size_t gj_str_binary_search(const char *const *table, size_t n,
 *                               const char *key);
 *     — Binary-search a sorted (strcmp ascending) table[0..n) of
 *       NUL-terminated C strings for key. Return any matching index,
 *       or n if not found. NULL table/key with n > 0 → n. n == 0 → 0.
 *       NULL table entries compare less than any non-NULL key string.
 *   size_t __gj_str_binary_search  (alias)
 *   __libcgj_batch1286_marker = "libcgj-batch1286"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1286_marker[] = "libcgj-batch1286";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1286_strcmp — lexicographic compare of two C strings (or NULL).
 * Returns <0 if a < b, 0 if equal, >0 if a > b. NULL < non-NULL.
 */
static int
b1286_strcmp(const char *szA, const char *szB)
{
	const unsigned char *pA;
	const unsigned char *pB;

	if (szA == szB) {
		return 0;
	}
	if (szA == NULL) {
		return -1;
	}
	if (szB == NULL) {
		return 1;
	}

	pA = (const unsigned char *)szA;
	pB = (const unsigned char *)szB;
	while (*pA != 0u && *pA == *pB) {
		pA++;
		pB++;
	}
	return (int)(*pA) - (int)(*pB);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_binary_search — index of key in sorted string table, or n.
 */
size_t
gj_str_binary_search(const char *const *ppTable, size_t cN,
    const char *szKey)
{
	size_t iLo;
	size_t iHi;

	if (cN == 0u) {
		return 0u;
	}
	if (ppTable == NULL || szKey == NULL) {
		return cN;
	}

	iLo = 0u;
	iHi = cN;
	while (iLo < iHi) {
		size_t iMid = iLo + (iHi - iLo) / 2u;
		int nCmp = b1286_strcmp(ppTable[iMid], szKey);

		if (nCmp < 0) {
			iLo = iMid + 1u;
		} else if (nCmp > 0) {
			iHi = iMid;
		} else {
			return iMid;
		}
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_binary_search(const char *const *ppTable, size_t cN,
    const char *szKey)
    __attribute__((alias("gj_str_binary_search")));
