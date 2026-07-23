/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch195: Cookie header name=value get.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party cookie source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   gj_http_header_get  → graph_batch114.c  (HTTP message header lookup)
 *   gj_url_query_get    → graph_batch167.c  (URL query key=value)
 *
 * This TU adds only unique symbols:
 *   int gj_cookie_get(const char *header, const char *name,
 *                     char *out, size_t cap);
 *     — Look up the first cookie named `name` in a Cookie header value
 *       (the cookie-string, without a leading "Cookie:" field-name).
 *       Pairs are separated by ';'. Optional SP/HTAB around separators
 *       and around the name/value sides of '=' are trimmed (RFC 6265
 *       cookie-string is typically "a=1; b=2"). Within a pair, the first
 *       '=' splits name from value; the value may be empty. A segment
 *       with no '=' is treated as a bare name with empty value.
 *       Name match is exact byte-for-byte (case-sensitive, no decode).
 *       Value is copied raw (quotes not stripped). out receives a
 *       NUL-terminated value when found.
 *       Returns 0 on success, -1 on error (NULL args, empty name,
 *       not found, or value does not fit including NUL in cap).
 *   __gj_cookie_get  (alias)
 *   __libcgj_batch195_marker = "libcgj-batch195"
 *
 * Notes:
 *   - First matching name wins if the cookie is repeated.
 *   - No errno (freestanding-safe); callers use return codes only.
 *   - No malloc; works under -ffreestanding -msse2.
 */

#include <stddef.h>

const char __libcgj_batch195_marker[] = "libcgj-batch195";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b195_is_ows(unsigned char ch)
{
	return ch == (unsigned char)' ' || ch == (unsigned char)'\t';
}

/*
 * True if the name slice [pName, pName+cbName) equals szName (NUL-terminated).
 */
static int
b195_name_eq(const char *pName, size_t cbName, const char *szName)
{
	size_t i;

	if (pName == NULL || szName == NULL) {
		return 0;
	}
	for (i = 0u; i < cbName; i++) {
		if (szName[i] == '\0' || pName[i] != szName[i]) {
			return 0;
		}
	}
	return (szName[cbName] == '\0') ? 1 : 0;
}

/*
 * Copy cb bytes from pSrc into szOut (cap bytes including NUL).
 * Returns 0 on success, -1 if cb+1 > cap or args invalid.
 */
static int
b195_copy_out(char *szOut, size_t cbCap, const char *pSrc, size_t cb)
{
	size_t i;

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (cb >= cbCap) {
		return -1;
	}
	if (cb > 0u && pSrc == NULL) {
		return -1;
	}
	for (i = 0u; i < cb; i++) {
		szOut[i] = pSrc[i];
	}
	szOut[cb] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cookie_get — extract one cookie value by name from a Cookie header
 * cookie-string (e.g. "session=abc; theme=dark").
 *
 * Walks ';'-separated pairs, trimming OWS around separators and around
 * the name / value. On match, writes the value (possibly empty) into
 * out[0..cap).
 */
int
gj_cookie_get(const char *header, const char *name, char *out, size_t cap)
{
	const char *p;
	const char *pPair;
	const char *pEnd;
	const char *pEq;
	const char *pName;
	const char *pVal;
	size_t cbNameTok;
	size_t cbVal;

	if (header == NULL || name == NULL || out == NULL || cap == 0u) {
		return -1;
	}
	if (name[0] == '\0') {
		return -1;
	}

	p = header;
	while (*p != '\0') {
		/* Skip empty segments (";;") and leading OWS after ';'. */
		if (*p == ';' || b195_is_ows((unsigned char)*p)) {
			p++;
			continue;
		}

		pPair = p;
		/* Pair extends to next ';' or NUL. */
		while (*p != '\0' && *p != ';') {
			p++;
		}
		pEnd = p;

		/* Trim trailing OWS from the pair slice. */
		while (pEnd > pPair && b195_is_ows((unsigned char)pEnd[-1])) {
			pEnd--;
		}

		/* Locate first '=' within this pair (if any). */
		pEq = pPair;
		while (pEq < pEnd && *pEq != '=') {
			pEq++;
		}

		if (pEq < pEnd && *pEq == '=') {
			/* Name: [pPair, pEq), trim trailing OWS before '='. */
			pName = pPair;
			cbNameTok = (size_t)(pEq - pPair);
			while (cbNameTok > 0u &&
			    b195_is_ows((unsigned char)pName[cbNameTok - 1u])) {
				cbNameTok--;
			}

			/* Value: (pEq+1, pEnd), trim leading OWS after '='. */
			pVal = pEq + 1;
			while (pVal < pEnd && b195_is_ows((unsigned char)*pVal)) {
				pVal++;
			}
			cbVal = (size_t)(pEnd - pVal);

			if (cbNameTok > 0u &&
			    b195_name_eq(pName, cbNameTok, name)) {
				return b195_copy_out(out, cap, pVal, cbVal);
			}
		} else {
			/* Bare name, empty value. pPair already left-trimmed. */
			cbNameTok = (size_t)(pEnd - pPair);
			if (cbNameTok > 0u &&
			    b195_name_eq(pPair, cbNameTok, name)) {
				return b195_copy_out(out, cap, "", 0u);
			}
		}

		/* p is at ';' or NUL — loop advances past separator / OWS. */
	}

	return -1;
}

int __gj_cookie_get(const char *header, const char *name, char *out,
    size_t cap)
    __attribute__((alias("gj_cookie_get")));
