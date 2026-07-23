/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1394: expand tab characters to spaces.
 *
 * Surface (unique symbols):
 *   int gj_str_expand_tabs(const char *s, size_t tabwidth,
 *                          char *out, size_t cap);
 *     — Copy s into out, replacing each '\t' with enough spaces to reach
 *       the next multiple of tabwidth (column positions, 0-based).
 *       tabwidth == 0 is treated as 1. Returns length excl NUL on success,
 *       or -1 if out == NULL, cap == 0, s == NULL, or the expanded result
 *       (plus NUL) does not fit in cap.
 *   int __gj_str_expand_tabs  (alias)
 *   __libcgj_batch1394_marker = "libcgj-batch1394"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1394_marker[] = "libcgj-batch1394";

/* ---- freestanding helpers ---------------------------------------------- */

/* Spaces needed to advance column col to the next tab stop of width tw. */
static size_t
b1394_tab_spaces(size_t iCol, size_t cbTab)
{
	size_t iMod;

	if (cbTab == 0u) {
		cbTab = 1u;
	}
	iMod = iCol % cbTab;
	return cbTab - iMod;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_expand_tabs — expand '\t' in sz to spaces of width cbTab.
 */
int
gj_str_expand_tabs(const char *sz, size_t cbTab, char *pOut, size_t cbCap)
{
	size_t iCol;
	size_t iOut;
	size_t iSp;
	size_t cbNeed;
	size_t cbSp;
	const unsigned char *p;

	if (pOut == NULL || cbCap == 0u || sz == NULL) {
		return -1;
	}

	if (cbTab == 0u) {
		cbTab = 1u;
	}

	/* First pass: compute expanded length. */
	iCol = 0u;
	cbNeed = 0u;
	p = (const unsigned char *)sz;
	while (*p != 0u) {
		if (*p == (unsigned char)'\t') {
			cbSp = b1394_tab_spaces(iCol, cbTab);
			if (cbSp > (SIZE_MAX - cbNeed)) {
				return -1;
			}
			cbNeed += cbSp;
			iCol += cbSp;
		} else {
			if (cbNeed == SIZE_MAX) {
				return -1;
			}
			cbNeed++;
			if (*p == (unsigned char)'\n' ||
			    *p == (unsigned char)'\r') {
				iCol = 0u;
			} else {
				iCol++;
			}
		}
		p++;
	}

	if (cbNeed + 1u < cbNeed || cbNeed + 1u > cbCap) {
		return -1;
	}

	/* Second pass: emit. */
	iCol = 0u;
	iOut = 0u;
	p = (const unsigned char *)sz;
	while (*p != 0u) {
		if (*p == (unsigned char)'\t') {
			cbSp = b1394_tab_spaces(iCol, cbTab);
			for (iSp = 0u; iSp < cbSp; iSp++) {
				pOut[iOut++] = ' ';
			}
			iCol += cbSp;
		} else {
			pOut[iOut++] = (char)*p;
			if (*p == (unsigned char)'\n' ||
			    *p == (unsigned char)'\r') {
				iCol = 0u;
			} else {
				iCol++;
			}
		}
		p++;
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_expand_tabs(const char *sz, size_t cbTab, char *pOut,
    size_t cbCap)
    __attribute__((alias("gj_str_expand_tabs")));
