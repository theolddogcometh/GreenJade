/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch168: HTML entity escape / unescape (lite).
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; this TU does not require SSE.
 *
 * Surface (unique symbols):
 *   int gj_html_escape(const char *in, char *out, size_t cap);
 *     Escape text for safe HTML text/attribute contexts:
 *       & → &amp;   < → &lt;   > → &gt;   " → &quot;   ' → &#39;
 *     All other octets pass through unchanged. out is always NUL-terminated
 *     on success. cap includes room for the trailing NUL.
 *     Returns 0 on success, -1 on error (NULL args or insufficient cap).
 *
 *   int gj_html_unescape(const char *in, char *out, size_t cap);
 *     Reverse the five entities above (exact lowercase match).
 *     Unknown or incomplete entity sequences are copied through as-is.
 *     Returns 0 on success, -1 on error (NULL args or insufficient cap).
 *
 *   __gj_html_escape / __gj_html_unescape  (aliases)
 *   __libcgj_batch168_marker = "libcgj-batch168"
 *
 * No errno (freestanding-safe); callers use return codes only.
 * No third-party HTML entity source was copied.
 */

#include <stddef.h>

const char __libcgj_batch168_marker[] = "libcgj-batch168";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b168_strlen(const char *sz)
{
	size_t n = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[n] != '\0') {
		n++;
	}
	return n;
}

/*
 * Append cb bytes from pSrc into szOut at *pIOut if room remains for those
 * bytes plus a final NUL (i.e. *pIOut + cb < cap). Advances *pIOut.
 * Returns 0 on success, -1 if out of space.
 */
static int
b168_emit(char *szOut, size_t cbCap, size_t *pIOut, const char *pSrc,
          size_t cb)
{
	size_t i;
	size_t iOut = *pIOut;

	if (cb > 0u && pSrc == NULL) {
		return -1;
	}
	if (iOut + cb >= cbCap) {
		return -1;
	}
	for (i = 0u; i < cb; i++) {
		szOut[iOut + i] = pSrc[i];
	}
	*pIOut = iOut + cb;
	return 0;
}

/* Match entity at p (length nEnt including leading '&'); 1 if exact. */
static int
b168_match(const char *p, size_t cbLeft, const char *szEnt, size_t nEnt)
{
	size_t i;

	if (cbLeft < nEnt) {
		return 0;
	}
	for (i = 0u; i < nEnt; i++) {
		if (p[i] != szEnt[i]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public: gj_html_escape -------------------------------------------- */

/*
 * Escape &, <, >, ", ' into the five listed HTML entities.
 * cap includes the trailing NUL. Returns 0 / -1.
 */
int
gj_html_escape(const char *szIn, char *szOut, size_t cbCap)
{
	size_t iIn;
	size_t iOut;
	size_t cbIn;
	unsigned char ch;
	const char *pRep;
	size_t nRep;

	if (szIn == NULL || szOut == NULL) {
		return -1;
	}
	if (cbCap == 0u) {
		return -1;
	}

	cbIn = b168_strlen(szIn);
	iOut = 0u;
	for (iIn = 0u; iIn < cbIn; iIn++) {
		ch = (unsigned char)szIn[iIn];
		switch (ch) {
		case (unsigned char)'&':
			pRep = "&amp;";
			nRep = 5u;
			break;
		case (unsigned char)'<':
			pRep = "&lt;";
			nRep = 4u;
			break;
		case (unsigned char)'>':
			pRep = "&gt;";
			nRep = 4u;
			break;
		case (unsigned char)'"':
			pRep = "&quot;";
			nRep = 6u;
			break;
		case (unsigned char)'\'':
			pRep = "&#39;";
			nRep = 5u;
			break;
		default:
			pRep = (const char *)&szIn[iIn];
			nRep = 1u;
			break;
		}
		if (b168_emit(szOut, cbCap, &iOut, pRep, nRep) != 0) {
			return -1;
		}
	}
	szOut[iOut] = '\0';
	return 0;
}

int __gj_html_escape(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_html_escape")));

/* ---- public: gj_html_unescape ------------------------------------------ */

/*
 * Unescape &amp; &lt; &gt; &quot; &#39; only (exact lowercase forms).
 * Other text, including bare '&' and unknown entities, is copied through.
 * cap includes the trailing NUL. Returns 0 / -1.
 */
int
gj_html_unescape(const char *szIn, char *szOut, size_t cbCap)
{
	size_t iIn;
	size_t iOut;
	size_t cbIn;
	size_t cbLeft;
	const char *p;
	char chOne;
	const char *pRep;
	size_t nEnt;
	size_t nRep;

	if (szIn == NULL || szOut == NULL) {
		return -1;
	}
	if (cbCap == 0u) {
		return -1;
	}

	cbIn = b168_strlen(szIn);
	iOut = 0u;
	iIn = 0u;
	while (iIn < cbIn) {
		p = szIn + iIn;
		cbLeft = cbIn - iIn;

		if (p[0] == '&') {
			/* Longest-first among known entities. */
			if (b168_match(p, cbLeft, "&quot;", 6u)) {
				pRep = "\"";
				nRep = 1u;
				nEnt = 6u;
			} else if (b168_match(p, cbLeft, "&amp;", 5u)) {
				pRep = "&";
				nRep = 1u;
				nEnt = 5u;
			} else if (b168_match(p, cbLeft, "&#39;", 5u)) {
				pRep = "'";
				nRep = 1u;
				nEnt = 5u;
			} else if (b168_match(p, cbLeft, "&lt;", 4u)) {
				pRep = "<";
				nRep = 1u;
				nEnt = 4u;
			} else if (b168_match(p, cbLeft, "&gt;", 4u)) {
				pRep = ">";
				nRep = 1u;
				nEnt = 4u;
			} else {
				/* Bare '&' or unknown entity: copy one char. */
				chOne = '&';
				pRep = &chOne;
				nRep = 1u;
				nEnt = 1u;
			}
		} else {
			chOne = p[0];
			pRep = &chOne;
			nRep = 1u;
			nEnt = 1u;
		}

		if (b168_emit(szOut, cbCap, &iOut, pRep, nRep) != 0) {
			return -1;
		}
		iIn += nEnt;
	}
	szOut[iOut] = '\0';
	return 0;
}

int __gj_html_unescape(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_html_unescape")));
