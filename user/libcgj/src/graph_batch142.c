/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch142: minimal freestanding YAML helpers for
 * flat key: value lines only. Clean-room pure C — integer/pointer only
 * (builds with -msse2; no SSE required). No third-party YAML source was
 * copied.
 *
 * Surface:
 *   int gj_yaml_get_string(const char *text, const char *key,
 *                          char *out, size_t cap);
 *   int gj_yaml_get_int(const char *text, const char *key, long *out);
 *   __gj_yaml_get_string / __gj_yaml_get_int  (aliases)
 *   __libcgj_batch142_marker = "libcgj-batch142"
 *
 * Semantics:
 *   get_string / get_int: 0 on success, -1 on error (errno set).
 *   Flat document only — top-level lines of the form:
 *     key: value
 *   Bare keys: [A-Za-z0-9_-]+ (no leading indentation — nested keys
 *   are skipped). Optional spaces around ':'.
 *   String values:
 *     - double-quoted with \\ \" \n \t \r \b \f \/ (YAML 1.1 subset)
 *     - single-quoted ('' → literal '); no other escapes
 *     - plain scalar: text to EOL, horizontal ws trimmed; trailing
 *       # comments only when preceded by horizontal whitespace
 *   Integer values: optional leading +/- and decimal digits only
 *     (quoted or plain).
 *   Blank lines, full-line # comments, document markers --- / ... are
 *   skipped. Nested mappings, sequences, multi-line scalars, tags,
 *   and anchors are not supported.
 *   errno: EINVAL bad input/syntax, ENOENT missing key, ENOSPC buffer,
 *          ERANGE integer overflow.
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch142_marker[] = "libcgj-batch142";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b142_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

static int
b142_is_eol(unsigned char ch)
{
	return ch == '\0' || ch == '\n' || ch == '\r';
}

static int
b142_is_bare_key_ch(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
	       (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
}

static int
b142_key_eq(const char *pKey, size_t cbKey, const char *szKey)
{
	size_t i;

	if (szKey == NULL) {
		return 0;
	}
	for (i = 0; i < cbKey; i++) {
		if (szKey[i] == '\0' || pKey[i] != szKey[i]) {
			return 0;
		}
	}
	return szKey[cbKey] == '\0';
}

/*
 * Advance past horizontal whitespace (not newline).
 */
static const char *
b142_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b142_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

/*
 * Skip to start of next line (past \r?\n). Returns pointer at next line
 * or at the terminating NUL.
 */
static const char *
b142_next_line(const char *pSz)
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
 * True if the line (already at first non-ws) is a document start/end
 * marker: "---" or "..." optionally followed by ws/comment/EOL.
 */
static int
b142_is_doc_marker(const char *p)
{
	if (p == NULL) {
		return 0;
	}
	if ((p[0] == '-' && p[1] == '-' && p[2] == '-') ||
	    (p[0] == '.' && p[1] == '.' && p[2] == '.')) {
		const char *q = p + 3;

		q = b142_skip_ws(q);
		return b142_is_eol((unsigned char)*q) || *q == '#';
	}
	return 0;
}

/*
 * Decode one double-quoted YAML string (opening '"' already consumed).
 * Writes unescaped bytes to pOut if non-NULL.
 * *pcbNeed receives unescaped length (no NUL).
 * On success returns pointer past closing '"'; on error returns NULL.
 * *pfOverflow is set when pOut is too small (vs bad syntax).
 */
static const char *
b142_parse_dquote(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed,
                  int *pfOverflow)
{
	size_t cb = 0;

	if (pfOverflow != NULL) {
		*pfOverflow = 0;
	}
	if (pSz == NULL) {
		return NULL;
	}

	while (*pSz != '\0' && *pSz != '"' && *pSz != '\n' && *pSz != '\r') {
		unsigned char ch = (unsigned char)*pSz;
		unsigned char chOut;

		if (ch != (unsigned char)'\\') {
			if (pOut != NULL) {
				if (cb >= cbCap) {
					if (pfOverflow != NULL) {
						*pfOverflow = 1;
					}
					return NULL;
				}
				pOut[cb] = (char)ch;
			}
			cb++;
			pSz++;
			continue;
		}

		pSz++;
		if (*pSz == '\0' || *pSz == '\n' || *pSz == '\r') {
			return NULL;
		}
		ch = (unsigned char)*pSz;
		pSz++;

		switch (ch) {
		case '"':
		case '\\':
		case '/':
			chOut = ch;
			break;
		case 'b':
			chOut = '\b';
			break;
		case 'f':
			chOut = '\f';
			break;
		case 'n':
			chOut = '\n';
			break;
		case 'r':
			chOut = '\r';
			break;
		case 't':
			chOut = '\t';
			break;
		default:
			/* unsupported escape (incl. \u / \x / \N) */
			return NULL;
		}

		if (pOut != NULL) {
			if (cb >= cbCap) {
				if (pfOverflow != NULL) {
					*pfOverflow = 1;
				}
				return NULL;
			}
			pOut[cb] = (char)chOut;
		}
		cb++;
	}

	if (*pSz != '"') {
		return NULL;
	}
	if (pcbNeed != NULL) {
		*pcbNeed = cb;
	}
	return pSz + 1;
}

/*
 * Decode one single-quoted YAML string (opening '\'' already consumed).
 * '' is the only escape (literal apostrophe).
 */
static const char *
b142_parse_squote(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed,
                  int *pfOverflow)
{
	size_t cb = 0;

	if (pfOverflow != NULL) {
		*pfOverflow = 0;
	}
	if (pSz == NULL) {
		return NULL;
	}

	while (*pSz != '\0' && *pSz != '\n' && *pSz != '\r') {
		unsigned char ch = (unsigned char)*pSz;

		if (ch == (unsigned char)'\'') {
			if (pSz[1] == '\'') {
				/* doubled quote → one apostrophe */
				if (pOut != NULL) {
					if (cb >= cbCap) {
						if (pfOverflow != NULL) {
							*pfOverflow = 1;
						}
						return NULL;
					}
					pOut[cb] = '\'';
				}
				cb++;
				pSz += 2;
				continue;
			}
			/* closing quote */
			if (pcbNeed != NULL) {
				*pcbNeed = cb;
			}
			return pSz + 1;
		}

		if (pOut != NULL) {
			if (cb >= cbCap) {
				if (pfOverflow != NULL) {
					*pfOverflow = 1;
				}
				return NULL;
			}
			pOut[cb] = (char)ch;
		}
		cb++;
		pSz++;
	}

	return NULL; /* unterminated */
}

/*
 * Capture a plain YAML scalar from pVal to EOL (exclusive of trailing
 * ws and optional " # comment"). Does not write; sets *ppEnd.
 */
static void
b142_plain_span(const char *pVal, const char **ppEnd)
{
	const char *pScan;
	const char *pEnd;

	pScan = pVal;
	while (!b142_is_eol((unsigned char)*pScan)) {
		if (*pScan == '#' && pScan > pVal &&
		    b142_is_ws((unsigned char)pScan[-1])) {
			break;
		}
		pScan++;
	}
	pEnd = pScan;
	while (pEnd > pVal && b142_is_ws((unsigned char)pEnd[-1])) {
		pEnd--;
	}
	if (ppEnd != NULL) {
		*ppEnd = pEnd;
	}
}

/*
 * Locate value text after "key:" for the first matching bare key at
 * column 0 (no indent). On success: *ppVal points at first non-ws char
 * of the value (may be EOL for empty); return 0.
 * On missing key: errno = ENOENT, return -1.
 * On bad document/key: errno = EINVAL, return -1.
 */
static int
b142_find_key(const char *szText, const char *szKey, const char **ppVal)
{
	const char *pLine;

	if (szText == NULL || szKey == NULL || ppVal == NULL ||
	    szKey[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	*ppVal = NULL;
	pLine = szText;

	while (*pLine != '\0') {
		const char *p;
		const char *pKeyStart;
		size_t cbKey;

		/* nested / indented content — skip (flat top-level only) */
		if (b142_is_ws((unsigned char)*pLine)) {
			pLine = b142_next_line(pLine);
			continue;
		}

		p = pLine;

		/* blank line */
		if (b142_is_eol((unsigned char)*p)) {
			pLine = b142_next_line(p);
			continue;
		}

		/* full-line comment */
		if (*p == '#') {
			pLine = b142_next_line(p);
			continue;
		}

		/* document markers */
		if (b142_is_doc_marker(p)) {
			pLine = b142_next_line(p);
			continue;
		}

		/* list / flow indicators at column 0 — skip line */
		if (*p == '-' || *p == '[' || *p == '{' || *p == '|' ||
		    *p == '>') {
			/* still allow bare keys that start with letter/digit */
			if (!b142_is_bare_key_ch((unsigned char)*p)) {
				pLine = b142_next_line(p);
				continue;
			}
		}

		/* bare key */
		if (!b142_is_bare_key_ch((unsigned char)*p)) {
			pLine = b142_next_line(p);
			continue;
		}
		pKeyStart = p;
		while (b142_is_bare_key_ch((unsigned char)*p)) {
			p++;
		}
		cbKey = (size_t)(p - pKeyStart);

		p = b142_skip_ws(p);
		if (*p != ':') {
			/* not a key: value assignment line */
			pLine = b142_next_line(p);
			continue;
		}
		p++; /* skip ':' */
		p = b142_skip_ws(p);

		if (b142_key_eq(pKeyStart, cbKey, szKey)) {
			*ppVal = p;
			return 0;
		}

		pLine = b142_next_line(p);
	}

	errno = ENOENT;
	return -1;
}

/*
 * After a successful quoted parse, require only trailing ws / # / EOL.
 */
static int
b142_trail_ok(const char *pEnd)
{
	pEnd = b142_skip_ws(pEnd);
	return b142_is_eol((unsigned char)*pEnd) || *pEnd == '#';
}

/* ---- public: flat key: value getters ----------------------------------- */

/*
 * Extract string value for key from flat key: value lines.
 * Unescapes quoted forms into out (NUL-terminated). outcap includes
 * room for NUL. Plain scalars are copied as-is (trimmed).
 */
int
gj_yaml_get_string(const char *szText, const char *szKey, char *szOut,
                   size_t cbOut)
{
	const char *pVal;
	const char *pEnd;
	size_t cbNeed = 0;
	int fOverflow = 0;
	size_t i;

	if (szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b142_find_key(szText, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b142_skip_ws(pVal);

	/* double-quoted */
	if (*pVal == '"') {
		pVal++;
		pEnd = b142_parse_dquote(pVal, szOut, cbOut - 1u, &cbNeed,
		                         &fOverflow);
		if (pEnd == NULL) {
			if (fOverflow) {
				errno = ENOSPC;
			} else {
				size_t cbProbe = 0;
				int fOv2 = 0;
				const char *pOk = b142_parse_dquote(
				    pVal, NULL, 0, &cbProbe, &fOv2);

				if (pOk != NULL && cbProbe + 1u > cbOut) {
					errno = ENOSPC;
				} else {
					errno = EINVAL;
				}
			}
			return -1;
		}
		if (!b142_trail_ok(pEnd)) {
			errno = EINVAL;
			return -1;
		}
		szOut[cbNeed] = '\0';
		return 0;
	}

	/* single-quoted */
	if (*pVal == '\'') {
		pVal++;
		pEnd = b142_parse_squote(pVal, szOut, cbOut - 1u, &cbNeed,
		                         &fOverflow);
		if (pEnd == NULL) {
			if (fOverflow) {
				errno = ENOSPC;
			} else {
				size_t cbProbe = 0;
				int fOv2 = 0;
				const char *pOk = b142_parse_squote(
				    pVal, NULL, 0, &cbProbe, &fOv2);

				if (pOk != NULL && cbProbe + 1u > cbOut) {
					errno = ENOSPC;
				} else {
					errno = EINVAL;
				}
			}
			return -1;
		}
		if (!b142_trail_ok(pEnd)) {
			errno = EINVAL;
			return -1;
		}
		szOut[cbNeed] = '\0';
		return 0;
	}

	/* plain scalar (may be empty) */
	{
		const char *pValEnd = pVal;

		b142_plain_span(pVal, &pValEnd);
		cbNeed = (size_t)(pValEnd - pVal);
		if (cbNeed + 1u > cbOut) {
			errno = ENOSPC;
			return -1;
		}
		for (i = 0; i < cbNeed; i++) {
			szOut[i] = pVal[i];
		}
		szOut[cbNeed] = '\0';
		return 0;
	}
}

/*
 * Extract integer value for key from flat key: 123 lines.
 * Accepts optional leading +/- and decimal digits only (no fraction/exp).
 * Value may be plain or double/single-quoted.
 */
int
gj_yaml_get_int(const char *szText, const char *szKey, long *pOut)
{
	const char *pVal;
	const char *pEnd;
	const char *p;
	const char *pNumEnd;
	char aTmp[64];
	size_t cbNeed = 0;
	int fOverflow = 0;
	int fNeg = 0;
	unsigned long uAcc = 0;
	int fDigits = 0;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b142_find_key(szText, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b142_skip_ws(pVal);

	/* resolve value span into a digit string pointer range */
	if (*pVal == '"') {
		pVal++;
		pEnd = b142_parse_dquote(pVal, aTmp, sizeof(aTmp) - 1u,
		                         &cbNeed, &fOverflow);
		if (pEnd == NULL || fOverflow || !b142_trail_ok(pEnd)) {
			errno = (fOverflow) ? ERANGE : EINVAL;
			return -1;
		}
		aTmp[cbNeed] = '\0';
		p = aTmp;
		pNumEnd = aTmp + cbNeed;
	} else if (*pVal == '\'') {
		pVal++;
		pEnd = b142_parse_squote(pVal, aTmp, sizeof(aTmp) - 1u,
		                         &cbNeed, &fOverflow);
		if (pEnd == NULL || fOverflow || !b142_trail_ok(pEnd)) {
			errno = (fOverflow) ? ERANGE : EINVAL;
			return -1;
		}
		aTmp[cbNeed] = '\0';
		p = aTmp;
		pNumEnd = aTmp + cbNeed;
	} else {
		const char *pValEnd = pVal;

		b142_plain_span(pVal, &pValEnd);
		p = pVal;
		pNumEnd = pValEnd;
	}

	/* skip leading horizontal ws inside resolved span */
	while (p < pNumEnd && b142_is_ws((unsigned char)*p)) {
		p++;
	}
	/* trim trailing ws inside span (quoted case) */
	while (pNumEnd > p && b142_is_ws((unsigned char)pNumEnd[-1])) {
		pNumEnd--;
	}

	if (p < pNumEnd && *p == '+') {
		p++;
	} else if (p < pNumEnd && *p == '-') {
		fNeg = 1;
		p++;
	}
	if (p >= pNumEnd || *p < '0' || *p > '9') {
		errno = EINVAL;
		return -1;
	}

	while (p < pNumEnd && *p >= '0' && *p <= '9') {
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

	/* reject fraction / exponent / leftover garbage */
	if (p != pNumEnd || !fDigits) {
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

int __gj_yaml_get_string(const char *szText, const char *szKey, char *szOut,
                         size_t cbOut)
    __attribute__((alias("gj_yaml_get_string")));

int __gj_yaml_get_int(const char *szText, const char *szKey, long *pOut)
    __attribute__((alias("gj_yaml_get_int")));
