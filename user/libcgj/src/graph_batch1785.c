/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1785: string-builder append uint64 decimal.
 *
 * Surface (unique symbols for this TU):
 *   typedef struct { char *buf; size_t len, cap; } gj_sb_t;
 *     — local typedef; layout matches batch1781.
 *   int gj_sb_append_u64_dec(gj_sb_t *sb, uint64_t v);
 *     — Append the base-10 representation of v (no leading zeros except
 *       for v == 0; max 20 digits for UINT64_MAX). All-or-nothing.
 *       No trailing NUL written. Returns 0 on success, -1 on error.
 *   int __gj_sb_append_u64_dec  (alias)
 *   __libcgj_batch1785_marker = "libcgj-batch1785"
 *
 * String-builder exclusive continuum 1781..1790. Distinct from
 * gj_format_u64_dec (batch391) which writes into a bare buffer.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1785_marker[] = "libcgj-batch1785";

/* ---- local state (plain struct typedef; layout = batch1781) ------------ */

typedef struct {
	char *buf;
	size_t len;
	size_t cap;
} gj_sb_t;

/* UINT64_MAX needs 20 decimal digits; reverse buffer holds that. */
#define B1785_MAX_DIGITS 20

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sb_append_u64_dec — append decimal digits of u64V onto the builder.
 *
 * Digits are formed LSD-first into a stack temp, then written MSD-first.
 */
int
gj_sb_append_u64_dec(gj_sb_t *pSb, uint64_t u64V)
{
	char aDig[B1785_MAX_DIGITS];
	int nDig;
	int iDig;
	uint64_t uVal;
	size_t cbCur;
	size_t cbAdd;
	size_t iOut;

	if (pSb == NULL || pSb->buf == NULL) {
		return -1;
	}

	uVal = u64V;
	nDig = 0;

	if (uVal == 0ull) {
		aDig[0] = '0';
		nDig = 1;
	} else {
		while (uVal > 0ull && nDig < B1785_MAX_DIGITS) {
			aDig[nDig++] = (char)('0' + (int)(uVal % 10ull));
			uVal /= 10ull;
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

int __gj_sb_append_u64_dec(gj_sb_t *pSb, uint64_t u64V)
    __attribute__((alias("gj_sb_append_u64_dec")));
