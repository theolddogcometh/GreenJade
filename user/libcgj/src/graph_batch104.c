/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch104: minimal freestanding INI helpers.
 * Clean-room pure C — integer/pointer only (no SSE required).
 * No third-party INI source was copied.
 *
 * Surface:
 *   gj_ini_get(const char *text, const char *section, const char *key,
 *              char *out, size_t outcap)
 *   gj_ini_get_int(const char *text, const char *section, const char *key,
 *                  long *out)
 *   __gj_ini_get / __gj_ini_get_int  (aliases)
 *   __libcgj_batch104_marker = "libcgj-batch104"
 *
 * Semantics:
 *   get / get_int: 0 on success, -1 on error (errno set).
 *   Sections: [name] (name matched exactly after trim of bracket contents).
 *   Keys: bare key=value lines (optional spaces around '=').
 *   Values: unquoted text to EOL; leading/trailing horizontal ws trimmed.
 *     Trailing # / ; comments only when preceded by horizontal whitespace.
 *   Full-line comments: lines whose first non-ws char is '#' or ';'.
 *   section == "" selects keys before the first [section] header (global).
 *   Blank lines ignored. Nested / multi-line values not supported.
 *   errno: EINVAL bad input/syntax, ENOENT missing section/key, ENOSPC
 *          buffer, ERANGE integer overflow.
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch104_marker[] = "libcgj-batch104";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b104_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

static int
b104_is_eol(unsigned char ch)
{
	return ch == '\0' || ch == '\n' || ch == '\r';
}

static int
b104_is_key_ch(unsigned char ch)
{
	/* bare INI key: alnum + common punctuation, not '=' / ws / comment */
	if (ch == '\0' || ch == '=' || ch == '[' || ch == ']' || ch == '#' ||
	    ch == ';' || b104_is_ws(ch) || ch == '\n' || ch == '\r') {
		return 0;
	}
	return 1;
}

static int
b104_streq_n(const char *pA, size_t cbA, const char *szB)
{
	size_t i;

	if (szB == NULL) {
		return 0;
	}
	for (i = 0; i < cbA; i++) {
		if (szB[i] == '\0' || pA[i] != szB[i]) {
			return 0;
		}
	}
	return szB[cbA] == '\0';
}

static const char *
b104_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b104_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

/*
 * Skip to start of next line (past \r?\n). Returns pointer at next line
 * or at the terminating NUL.
 */
static const char *
b104_next_line(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && *pSz != '\n' && *pSz != '\r') {
		pSz++;
	}
	if (*pSz == '\r') {
		pSz++;
	}
	if (*pSz == '\n') {
		pSz++;
	}
	return pSz;
}

/*
 * Parse [section] header. p points at '['.
 * On success: *ppName / *pcbName set to trimmed name span; returns pointer
 * past ']'. On syntax error returns NULL.
 */
static const char *
b104_parse_section(const char *p, const char **ppName, size_t *pcbName)
{
	const char *pName;
	const char *pEnd;
	const char *pClose;

	if (p == NULL || *p != '[') {
		return NULL;
	}
	p++;
	pName = b104_skip_ws(p);

	/* find closing ']' on this line */
	pClose = pName;
	while (*pClose != '\0' && *pClose != ']' && *pClose != '\n' &&
	       *pClose != '\r') {
		pClose++;
	}
	if (*pClose != ']') {
		return NULL;
	}

	/* trim trailing ws inside brackets */
	pEnd = pClose;
	while (pEnd > pName && b104_is_ws((unsigned char)pEnd[-1])) {
		pEnd--;
	}

	if (ppName != NULL) {
		*ppName = pName;
	}
	if (pcbName != NULL) {
		*pcbName = (size_t)(pEnd - pName);
	}
	return pClose + 1;
}

/*
 * Locate value text after "key=" for the first matching key inside the
 * requested section. On success: *ppVal points at first non-ws char of the
 * value span start (may be EOL for empty value); *ppValEnd exclusive end
 * of trimmed value; return 0.
 */
static int
b104_find_key(const char *szText, const char *szSection, const char *szKey,
              const char **ppVal, const char **ppValEnd)
{
	const char *pLine;
	int fInSection;

	if (szText == NULL || szSection == NULL || szKey == NULL ||
	    ppVal == NULL || ppValEnd == NULL || szKey[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	*ppVal = NULL;
	*ppValEnd = NULL;

	/* empty section name → global keys (before any header) */
	fInSection = (szSection[0] == '\0') ? 1 : 0;
	pLine = szText;

	while (*pLine != '\0') {
		const char *p;
		const char *pKeyStart;
		size_t cbKey;
		const char *pValStart;
		const char *pValEnd;
		const char *pScan;

		p = b104_skip_ws(pLine);

		/* blank line */
		if (b104_is_eol((unsigned char)*p)) {
			pLine = b104_next_line(p);
			continue;
		}

		/* full-line comment */
		if (*p == '#' || *p == ';') {
			pLine = b104_next_line(p);
			continue;
		}

		/* section header */
		if (*p == '[') {
			const char *pName = NULL;
			size_t cbName = 0;
			const char *pAfter;

			pAfter = b104_parse_section(p, &pName, &cbName);
			if (pAfter == NULL) {
				/* malformed header — skip line */
				pLine = b104_next_line(p);
				continue;
			}
			/* rest of line must be ws / comment / EOL */
			pAfter = b104_skip_ws(pAfter);
			if (!b104_is_eol((unsigned char)*pAfter) &&
			    *pAfter != '#' && *pAfter != ';') {
				pLine = b104_next_line(p);
				continue;
			}
			fInSection = b104_streq_n(pName, cbName, szSection);
			pLine = b104_next_line(p);
			continue;
		}

		if (!fInSection) {
			pLine = b104_next_line(p);
			continue;
		}

		/* bare key */
		if (!b104_is_key_ch((unsigned char)*p)) {
			pLine = b104_next_line(p);
			continue;
		}
		pKeyStart = p;
		while (b104_is_key_ch((unsigned char)*p)) {
			p++;
		}
		cbKey = (size_t)(p - pKeyStart);

		p = b104_skip_ws(p);
		if (*p != '=') {
			/* not a key=value assignment line */
			pLine = b104_next_line(p);
			continue;
		}
		p++; /* skip '=' */
		p = b104_skip_ws(p);
		pValStart = p;

		/* value runs to EOL; strip trailing comment if ws-prefixed */
		pScan = pValStart;
		while (!b104_is_eol((unsigned char)*pScan)) {
			if ((*pScan == '#' || *pScan == ';') &&
			    pScan > pValStart &&
			    b104_is_ws((unsigned char)pScan[-1])) {
				break;
			}
			pScan++;
		}
		pValEnd = pScan;

		/* trim trailing horizontal whitespace of value */
		while (pValEnd > pValStart &&
		       b104_is_ws((unsigned char)pValEnd[-1])) {
			pValEnd--;
		}

		if (b104_streq_n(pKeyStart, cbKey, szKey)) {
			*ppVal = pValStart;
			*ppValEnd = pValEnd;
			return 0;
		}

		pLine = b104_next_line(p);
	}

	errno = ENOENT;
	return -1;
}

/* ---- public: section/key getters --------------------------------------- */

/*
 * Extract string value for section/key. Copies trimmed value into out
 * (NUL-terminated). outcap includes room for NUL.
 */
int
gj_ini_get(const char *szText, const char *szSection, const char *szKey,
           char *szOut, size_t cbOut)
{
	const char *pVal;
	const char *pValEnd;
	size_t cb;
	size_t i;

	if (szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b104_find_key(szText, szSection, szKey, &pVal, &pValEnd) != 0) {
		return -1;
	}

	cb = (size_t)(pValEnd - pVal);
	if (cb + 1u > cbOut) {
		errno = ENOSPC;
		return -1;
	}

	for (i = 0; i < cb; i++) {
		szOut[i] = pVal[i];
	}
	szOut[cb] = '\0';
	return 0;
}

/*
 * Extract integer value for section/key.
 * Accepts optional leading +/- and decimal digits only (no fraction/exp).
 */
int
gj_ini_get_int(const char *szText, const char *szSection, const char *szKey,
               long *pOut)
{
	const char *pVal;
	const char *pValEnd;
	const char *p;
	int fNeg = 0;
	unsigned long uAcc = 0;
	int fDigits = 0;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b104_find_key(szText, szSection, szKey, &pVal, &pValEnd) != 0) {
		return -1;
	}

	p = pVal;
	if (p < pValEnd && *p == '+') {
		p++;
	} else if (p < pValEnd && *p == '-') {
		fNeg = 1;
		p++;
	}
	if (p >= pValEnd || *p < '0' || *p > '9') {
		errno = EINVAL;
		return -1;
	}

	while (p < pValEnd && *p >= '0' && *p <= '9') {
		unsigned long uDigit = (unsigned long)(*p - '0');

		if (uAcc > (unsigned long)(~0ul / 10ul)) {
			errno = ERANGE;
			return -1;
		}
		uAcc *= 10ul;
		if (uAcc > (unsigned long)(~0ul) - uDigit) {
			errno = ERANGE;
			return -1;
		}
		uAcc += uDigit;
		fDigits = 1;
		p++;
	}

	/* reject leftover non-digit content in the value span */
	if (p != pValEnd || !fDigits) {
		errno = EINVAL;
		return -1;
	}

	if (fNeg) {
		if (uAcc > (unsigned long)LONG_MAX + 1ul) {
			errno = ERANGE;
			return -1;
		}
		if (uAcc == (unsigned long)LONG_MAX + 1ul) {
			*pOut = (-LONG_MAX) - 1L;
		} else {
			*pOut = -(long)uAcc;
		}
	} else {
		if (uAcc > (unsigned long)LONG_MAX) {
			errno = ERANGE;
			return -1;
		}
		*pOut = (long)uAcc;
	}
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_ini_get(const char *szText, const char *szSection, const char *szKey,
                 char *szOut, size_t cbOut)
    __attribute__((alias("gj_ini_get")));

int __gj_ini_get_int(const char *szText, const char *szSection,
                     const char *szKey, long *pOut)
    __attribute__((alias("gj_ini_get_int")));
