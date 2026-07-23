/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2103: full L+R trim of ASCII whitespace on a span.
 *
 * Surface (unique symbols):
 *   int gj_str_trim_ascii_span(const char *s, size_t n,
 *                              size_t *out_off, size_t *out_len);
 *     - Compute the maximal subspan of s[0..n) with leading and trailing
 *       ASCII whitespace removed. On success writes *out_off (start
 *       offset into s) and *out_len (trimmed length) and returns 1 if
 *       the trimmed core is non-empty, else 0. Whitespace: space, HT,
 *       LF, VT, FF, CR (0x09..0x0D, 0x20). NULL out_off or out_len ->
 *       -1. s == NULL with n > 0 is treated as empty (off=0,len=0, ret 0).
 *   int __gj_str_trim_ascii_span  (alias)
 *   __libcgj_batch2103_marker = "libcgj-batch2103"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Distinct from
 * gj_str_trim (batch357, in-place NUL string) and gj_str_trim_copy
 * (batch903) - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2103_marker[] = "libcgj-batch2103";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2103_is_ws(unsigned char u8C)
{
	return (u8C == 0x20u || (u8C >= 0x09u && u8C <= 0x0du)) ? 1 : 0;
}

static int
b2103_trim(const char *sz, size_t cb, size_t *pOff, size_t *pLen)
{
	size_t iLo;
	size_t iHi;

	if (pOff == NULL || pLen == NULL) {
		return -1;
	}
	if (sz == NULL || cb == 0u) {
		*pOff = 0u;
		*pLen = 0u;
		return 0;
	}

	iLo = 0u;
	while (iLo < cb && b2103_is_ws((unsigned char)sz[iLo])) {
		iLo++;
	}
	if (iLo >= cb) {
		*pOff = cb;
		*pLen = 0u;
		return 0;
	}

	iHi = cb;
	while (iHi > iLo && b2103_is_ws((unsigned char)sz[iHi - 1u])) {
		iHi--;
	}

	*pOff = iLo;
	*pLen = iHi - iLo;
	return (*pLen > 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_trim_ascii_span - L+R strip ASCII ws; write core off/len.
 *
 * s: buffer of n octets, or NULL (empty)
 * n: span length
 * out_off: receives start offset into s (required)
 * out_len: receives trimmed length (required)
 *
 * Returns 1 if trimmed core non-empty, 0 if empty, -1 if out pointers
 * are NULL. Does not write through s. Does not call libc.
 */
int
gj_str_trim_ascii_span(const char *s, size_t n, size_t *out_off,
    size_t *out_len)
{
	return b2103_trim(s, n, out_off, out_len);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_trim_ascii_span(const char *s, size_t n, size_t *out_off,
    size_t *out_len)
    __attribute__((alias("gj_str_trim_ascii_span")));
