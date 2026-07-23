/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1786: string-builder append uint64 hex.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sb_append_hex_u64(gj_sb_t *sb, uint64_t v);
 *     — Append lowercase hex of v (no "0x"; no leading zeros except for
 *       v == 0; max 16 digits for UINT64_MAX). All-or-nothing. No
 *       trailing NUL written. Returns 0 on success, -1 on error.
 *   int __gj_sb_append_hex_u64  (alias)
 *   __libcgj_batch1786_marker = "libcgj-batch1786"
 *
 * String-builder exclusive continuum 1781..1790. Distinct from
 * gj_format_hex_u64 (batch586) which writes into a bare buffer.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1786_marker[] = "libcgj-batch1786";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* UINT64_MAX needs 16 hex digits; reverse buffer holds that. */
#define B1786_MAX_DIGITS 16

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map a nibble 0..15 to lowercase hex ASCII.
 */
static char
b1786_nibble(unsigned uN)
{
	if (uN < 10u) {
		return (char)('0' + (int)uN);
	}
	return (char)('a' + (int)(uN - 10u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_append_hex_u64 — append lowercase hex digits of u64V.
 *
 * Digits formed LSD-first into a stack temp, then written MSD-first.
 */
int
gj_sb_append_hex_u64(gj_sb_t *pSb, uint64_t u64V)
{
	char aDig[B1786_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t cbCur;
	size_t cbAdd;
	size_t iOut;
	unsigned uNibble;

	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}

	uVal = u64V;
	nDig = 0;

	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B1786_MAX_DIGITS) {
			uNibble = (unsigned)(uVal & 0xfull);
			aDig[nDig++] = b1786_nibble(uNibble);
			uVal >>= 4;
		}
	}

	cbAdd = (size_t)nDig;
	cbCur = pSb->len;
	if (cbCur > pSb->cap || cbAdd > pSb->cap - cbCur) {
		return -1;
	}

	iOut = cbCur;
	for (iDig = nDig; iDig > 0; iDig--) {
		pSb->buf[iOut++] = aDig[iDig - 1];
	}
	pSb->len = iOut;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sb_append_hex_u64(gj_sb_t *pSb, uint64_t u64V)
    __attribute__((alias("gj_sb_append_hex_u64")));
