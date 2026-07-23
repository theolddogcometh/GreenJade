/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1327: 3-way partition of a C string on the last separator match.
 *
 * Surface (unique symbols):
 *   int gj_str_rpartition(const char *s, const char *sep,
 *                         const char **before, size_t *before_len,
 *                         const char **mid, size_t *mid_len,
 *                         const char **after, size_t *after_len);
 *     — Like gj_str_partition but uses the last occurrence of sep.
 *       Returns 1 found / 0 not found / -1 FAIL (NULL args).
 *   int __gj_str_rpartition  (alias)
 *   __libcgj_batch1327_marker = "libcgj-batch1327"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1327_marker[] = "libcgj-batch1327";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1327_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static int
b1327_eq_n(const char *szA, const char *szB, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		if ((unsigned char)szA[i] != (unsigned char)szB[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_rpartition — last-separator 3-way view of s (non-destructive).
 */
int
gj_str_rpartition(const char *sz, const char *szSep,
    const char **ppBefore, size_t *pcbBefore,
    const char **ppMid, size_t *pcbMid,
    const char **ppAfter, size_t *pcbAfter)
{
	size_t cbS;
	size_t cbSep;
	size_t iPos;

	if (sz == NULL || szSep == NULL ||
	    ppBefore == NULL || pcbBefore == NULL ||
	    ppMid == NULL || pcbMid == NULL ||
	    ppAfter == NULL || pcbAfter == NULL) {
		return -1;
	}

	cbS = b1327_strlen(sz);
	cbSep = b1327_strlen(szSep);

	if (cbSep == 0u || cbSep > cbS) {
		*ppBefore = sz;
		*pcbBefore = cbS;
		*ppMid = sz + cbS;
		*pcbMid = 0u;
		*ppAfter = sz + cbS;
		*pcbAfter = 0u;
		return 0;
	}

	iPos = cbS - cbSep + 1u;
	while (iPos > 0u) {
		iPos--;
		if (b1327_eq_n(sz + iPos, szSep, cbSep)) {
			*ppBefore = sz;
			*pcbBefore = iPos;
			*ppMid = sz + iPos;
			*pcbMid = cbSep;
			*ppAfter = sz + iPos + cbSep;
			*pcbAfter = cbS - (iPos + cbSep);
			return 1;
		}
	}

	*ppBefore = sz;
	*pcbBefore = cbS;
	*ppMid = sz + cbS;
	*pcbMid = 0u;
	*ppAfter = sz + cbS;
	*pcbAfter = 0u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_rpartition(const char *sz, const char *szSep,
    const char **ppBefore, size_t *pcbBefore,
    const char **ppMid, size_t *pcbMid,
    const char **ppAfter, size_t *pcbAfter)
    __attribute__((alias("gj_str_rpartition")));
