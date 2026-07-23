/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2109: length after expanding tabs to spaces.
 *
 * Surface (unique symbols):
 *   size_t gj_str_tab_expand_len(const char *s, size_t n);
 *     - Return the octet length of s[0..n) if each HT (0x09) were
 *       replaced by enough spaces to reach the next multiple of 8
 *       columns (0-based column counter; non-HT octets advance the
 *       column by 1). s == NULL or n == 0 -> 0. Length-only; does not
 *       write memory. Distinct from gj_str_expand_tabs (batch1394,
 *       copy-into-out).
 *   size_t __gj_str_tab_expand_len  (alias)
 *   __libcgj_batch2109_marker = "libcgj-batch2109"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Unique
 * gj_str_tab_expand_len surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2109_marker[] = "libcgj-batch2109";

/* ---- freestanding helpers ---------------------------------------------- */

enum { b2109_tab_stop = 8 };

static size_t
b2109_expand_len(const char *sz, size_t cb)
{
	size_t i;
	size_t cOut;
	size_t cCol;
	size_t cPad;
	unsigned char u8C;

	if (sz == NULL || cb == 0u) {
		return 0u;
	}

	cOut = 0u;
	cCol = 0u;
	for (i = 0u; i < cb; i++) {
		u8C = (unsigned char)sz[i];
		if (u8C == (unsigned char)'\t') {
			cPad = (size_t)b2109_tab_stop
			    - (cCol % (size_t)b2109_tab_stop);
			if (cPad == 0u) {
				cPad = (size_t)b2109_tab_stop;
			}
			cOut += cPad;
			cCol += cPad;
		} else {
			cOut++;
			cCol++;
		}
	}
	return cOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_tab_expand_len - octet length after expanding HT to spaces.
 *
 * s: buffer of n octets (NULL -> 0)
 * n: span length
 *
 * Tab width is fixed at 8 columns. Non-tab octets count as one each
 * (including controls). Does not write through s. Does not call libc.
 */
size_t
gj_str_tab_expand_len(const char *s, size_t n)
{
	return b2109_expand_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_tab_expand_len(const char *s, size_t n)
    __attribute__((alias("gj_str_tab_expand_len")));
