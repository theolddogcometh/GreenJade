/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1326: 3-way partition of a C string on the first separator match.
 *
 * Surface (unique symbols):
 *   int gj_str_partition(const char *s, const char *sep,
 *                        const char **before, size_t *before_len,
 *                        const char **mid, size_t *mid_len,
 *                        const char **after, size_t *after_len);
 *     — Non-destructive. On first match of sep in s: before = s[0..pos),
 *       mid = s[pos..pos+seplen), after = s[pos+seplen..]. Returns 1 (OK
 *       found). No match / empty sep: before = whole s, mid/after empty
 *       lengths, returns 0. NULL s/sep or any NULL out pointer → -1 FAIL.
 *   int __gj_str_partition  (alias)
 *   __libcgj_batch1326_marker = "libcgj-batch1326"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1326_marker[] = "libcgj-batch1326";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1326_strlen(const char *sz)
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
b1326_eq_n(const char *szA, const char *szB, size_t cb)
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
 * gj_str_partition — first-separator 3-way view of s (non-destructive).
 */
int
gj_str_partition(const char *sz, const char *szSep,
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

	cbS = b1326_strlen(sz);
	cbSep = b1326_strlen(szSep);

	if (cbSep == 0u) {
		*ppBefore = sz;
		*pcbBefore = cbS;
		*ppMid = sz + cbS;
		*pcbMid = 0u;
		*ppAfter = sz + cbS;
		*pcbAfter = 0u;
		return 0;
	}

	if (cbSep <= cbS) {
		for (iPos = 0u; iPos + cbSep <= cbS; iPos++) {
			if (b1326_eq_n(sz + iPos, szSep, cbSep)) {
				*ppBefore = sz;
				*pcbBefore = iPos;
				*ppMid = sz + iPos;
				*pcbMid = cbSep;
				*ppAfter = sz + iPos + cbSep;
				*pcbAfter = cbS - (iPos + cbSep);
				return 1;
			}
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

int __gj_str_partition(const char *sz, const char *szSep,
    const char **ppBefore, size_t *pcbBefore,
    const char **ppMid, size_t *pcbMid,
    const char **ppAfter, size_t *pcbAfter)
    __attribute__((alias("gj_str_partition")));
