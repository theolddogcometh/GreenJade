/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch167: URL query-string parameter get.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party URL source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   gj_url_parse / gj_url_encode / gj_url_decode → graph_batch103.c
 *
 * This TU adds only unique symbols:
 *   int gj_url_query_get(const char *query, const char *key,
 *                        char *out, size_t cap);
 *     — Look up the first occurrence of key in a query string.
 *       query is WITHOUT a leading '?' (e.g. "a=1&b=two%20x&flag").
 *       Pairs are separated by '&'. Within a pair, key is everything
 *       before the first '=', value is everything after (may be empty).
 *       A pair with no '=' is a bare key with empty value.
 *       '#' ends the query scan (fragment not part of query).
 *       Key match is exact byte-for-byte (no percent-decode of keys).
 *       Value is copied raw (not percent-decoded); use gj_url_decode
 *       if needed. out receives a NUL-terminated value when found.
 *       Returns 0 on success, -1 on error (NULL args, empty key,
 *       not found, or value does not fit including NUL in cap).
 *   __gj_url_query_get  (alias)
 *   __libcgj_batch167_marker = "libcgj-batch167"
 *
 * Notes:
 *   - First matching key wins if the key is repeated.
 *   - No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>

const char __libcgj_batch167_marker[] = "libcgj-batch167";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * True if the key slice [pKey, pKey+cbKey) equals szKey (NUL-terminated).
 */
static int
b167_key_eq(const char *pKey, size_t cbKey, const char *szKey)
{
	size_t i;

	if (pKey == NULL || szKey == NULL) {
		return 0;
	}
	for (i = 0u; i < cbKey; i++) {
		if (szKey[i] == '\0' || pKey[i] != szKey[i]) {
			return 0;
		}
	}
	return (szKey[cbKey] == '\0') ? 1 : 0;
}

/*
 * Copy cb bytes from pSrc into szOut (cap bytes including NUL).
 * Returns 0 on success, -1 if cb+1 > cap.
 */
static int
b167_copy_out(char *szOut, size_t cbCap, const char *pSrc, size_t cb)
{
	size_t i;

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (cb >= cbCap) {
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
 * gj_url_query_get — extract one raw query parameter value by key.
 *
 * query has no leading '?'. Walks &-separated pairs until '#' or NUL.
 * On match, writes the value (possibly empty) into out[0..cap).
 */
int
gj_url_query_get(const char *szQuery, const char *szKey, char *szOut,
    size_t cbCap)
{
	const char *p;
	const char *pPair;
	const char *pEq;
	const char *pEnd;
	size_t cbKeyName;
	size_t cbVal;

	if (szQuery == NULL || szKey == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (szKey[0] == '\0') {
		return -1;
	}

	p = szQuery;
	while (*p != '\0' && *p != '#') {
		/* Skip empty pairs ("&&" or leading/trailing '&'). */
		if (*p == '&') {
			p++;
			continue;
		}

		pPair = p;
		/* Pair extends to next '&', '#', or NUL. */
		while (*p != '\0' && *p != '&' && *p != '#') {
			p++;
		}
		pEnd = p;

		/* Locate first '=' within this pair (if any). */
		pEq = pPair;
		while (pEq < pEnd && *pEq != '=') {
			pEq++;
		}

		if (pEq < pEnd && *pEq == '=') {
			cbKeyName = (size_t)(pEq - pPair);
			if (b167_key_eq(pPair, cbKeyName, szKey)) {
				cbVal = (size_t)(pEnd - (pEq + 1));
				return b167_copy_out(szOut, cbCap, pEq + 1,
				    cbVal);
			}
		} else {
			/* Bare key, empty value. */
			cbKeyName = (size_t)(pEnd - pPair);
			if (b167_key_eq(pPair, cbKeyName, szKey)) {
				return b167_copy_out(szOut, cbCap, "", 0u);
			}
		}

		/* p is at '&', '#', or NUL — loop advances past '&'. */
	}

	return -1;
}

int __gj_url_query_get(const char *szQuery, const char *szKey, char *szOut,
    size_t cbCap)
    __attribute__((alias("gj_url_query_get")));
