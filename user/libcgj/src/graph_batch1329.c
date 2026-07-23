/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1329: join an array of C strings with a separator into a bounded buffer.
 *
 * Surface (unique symbols):
 *   int gj_str_join_n(const char *const *strs, size_t n, const char *sep,
 *                     char *out, size_t cap);
 *     — Concatenate strs[0..n) into out with sep between elements.
 *       Returns 0 (OK) on success (NUL-terminated result fits in cap);
 *       -1 (FAIL) on NULL out, cap == 0, strs == NULL with n > 0, size
 *       overflow, or insufficient cap. NULL sep / NULL strs[i] → "".
 *       n == 0 → empty string when cap allows a lone NUL.
 *   int __gj_str_join_n  (alias)
 *
 * Distinct from gj_join (batch354) / gj_str_join_path* — unique name.
 *   __libcgj_batch1329_marker = "libcgj-batch1329"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1329_marker[] = "libcgj-batch1329";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1329_len(const char *sz)
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
b1329_add(size_t *pCb, size_t cbAdd)
{
	if (cbAdd > (size_t)-1 - *pCb) {
		return -1;
	}
	*pCb += cbAdd;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_join_n — join n strings with sep into out[cap].
 */
int
gj_str_join_n(const char *const *ppStrs, size_t cN, const char *szSep,
    char *szOut, size_t cbCap)
{
	const char *szS;
	size_t cbSep;
	size_t cbNeed;
	size_t iPart;
	size_t iOut;
	size_t cbPart;
	size_t iByte;
	const char *szPart;

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (cN > 0u && ppStrs == NULL) {
		return -1;
	}

	szS = (szSep != NULL) ? szSep : "";
	cbSep = b1329_len(szS);

	cbNeed = 0u;
	for (iPart = 0u; iPart < cN; iPart++) {
		szPart = ppStrs[iPart];
		cbPart = b1329_len(szPart);
		if (b1329_add(&cbNeed, cbPart) != 0) {
			return -1;
		}
		if (iPart + 1u < cN) {
			if (b1329_add(&cbNeed, cbSep) != 0) {
				return -1;
			}
		}
	}

	if (cbNeed + 1u < cbNeed || cbNeed + 1u > cbCap) {
		return -1;
	}

	iOut = 0u;
	for (iPart = 0u; iPart < cN; iPart++) {
		szPart = ppStrs[iPart];
		cbPart = b1329_len(szPart);
		for (iByte = 0u; iByte < cbPart; iByte++) {
			szOut[iOut++] = szPart[iByte];
		}
		if (iPart + 1u < cN) {
			for (iByte = 0u; iByte < cbSep; iByte++) {
				szOut[iOut++] = szS[iByte];
			}
		}
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_join_n(const char *const *ppStrs, size_t cN, const char *szSep,
    char *szOut, size_t cbCap)
    __attribute__((alias("gj_str_join_n")));
