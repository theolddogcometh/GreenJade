/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1391: bounded string repeat (fill by repeating s).
 *
 * Surface (unique symbols):
 *   int gj_str_repeat_n(const char *s, size_t n, char *out, size_t cap);
 *     — Fill out by writing s repeated n times, truncated to fit in
 *       cap (including room for a trailing NUL when cap > 0). Returns
 *       length written excluding NUL on success, or -1 on error
 *       (out == NULL, cap == 0, or s == NULL with n > 0 and need > 0).
 *       n == 0 or empty s → empty string (length 0) when cap allows.
 *       Distinct from gj_str_repeat (batch1081) which fails instead of
 *       truncating when the full product does not fit.
 *   int __gj_str_repeat_n  (alias)
 *   __libcgj_batch1391_marker = "libcgj-batch1391"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1391_marker[] = "libcgj-batch1391";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1391_strlen(const char *sz)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_repeat_n — write sz repeated cN times into pOut, cap-bounded.
 *
 * Truncates rather than failing when the full expansion exceeds cap-1.
 */
int
gj_str_repeat_n(const char *sz, size_t cN, char *pOut, size_t cbCap)
{
	size_t cbSrc;
	size_t cbMax;
	size_t iOut;
	size_t iRep;
	size_t iCh;

	if (pOut == NULL || cbCap == 0u) {
		return -1;
	}

	if (cN == 0u) {
		pOut[0] = '\0';
		return 0;
	}

	if (sz == NULL) {
		return -1;
	}

	cbSrc = b1391_strlen(sz);
	if (cbSrc == 0u) {
		pOut[0] = '\0';
		return 0;
	}

	cbMax = cbCap - 1u;
	iOut = 0u;
	for (iRep = 0u; iRep < cN && iOut < cbMax; iRep++) {
		for (iCh = 0u; iCh < cbSrc && iOut < cbMax; iCh++) {
			pOut[iOut++] = sz[iCh];
		}
	}
	pOut[iOut] = '\0';
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_repeat_n(const char *sz, size_t cN, char *pOut, size_t cbCap)
    __attribute__((alias("gj_str_repeat_n")));
