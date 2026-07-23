/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2104: length after collapsing whitespace runs.
 *
 * Surface (unique symbols):
 *   size_t gj_str_collapse_ws_len(const char *s, size_t n);
 *     - Return the octet length of s[0..n) if every run of one or more
 *       ASCII whitespace octets were replaced by a single space (0x20).
 *       Does not write memory; length-only probe. Whitespace: space, HT,
 *       LF, VT, FF, CR (0x09..0x0D, 0x20). s == NULL or n == 0 -> 0.
 *   size_t __gj_str_collapse_ws_len  (alias)
 *   __libcgj_batch2104_marker = "libcgj-batch2104"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_collapse_ws (batch1203, copy-into-out) - unique length surface;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2104_marker[] = "libcgj-batch2104";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2104_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

static size_t
b2104_collapse_len(const char *sz, size_t cb)
{
	size_t i;
	size_t cOut;
	int fInWs;

	if (sz == NULL || cb == 0u) {
		return 0u;
	}

	cOut = 0u;
	fInWs = 0;
	for (i = 0u; i < cb; i++) {
		if (b2104_is_ws((unsigned char)sz[i])) {
			if (fInWs == 0) {
				cOut++;
				fInWs = 1;
			}
		} else {
			cOut++;
			fInWs = 0;
		}
	}
	return cOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_collapse_ws_len - octet length after collapsing ws runs to ' '.
 *
 * s: buffer of n octets (NULL -> 0)
 * n: span length
 *
 * Leading/trailing whitespace each contribute a single space if present.
 * Does not write through s. Does not call libc.
 */
size_t
gj_str_collapse_ws_len(const char *s, size_t n)
{
	return b2104_collapse_len(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_str_collapse_ws_len(const char *s, size_t n)
    __attribute__((alias("gj_str_collapse_ws_len")));
