/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch81: minimal freestanding TOML helpers for
 * flat key=value lines only. Clean-room pure C — integer/pointer only
 * (no SSE). No third-party TOML source was copied.
 *
 * Surface:
 *   gj_toml_get_string(const char *toml, const char *key,
 *                      char *out, size_t outcap)
 *   gj_toml_get_int(const char *toml, const char *key, long *out)
 *   gj_toml_has_key(const char *toml, const char *key) → 1/0
 *   __gj_toml_get_string / __gj_toml_get_int / __gj_toml_has_key
 *   __libcgj_batch81_marker = "libcgj-batch81"
 *
 * Semantics:
 *   get_string / get_int: 0 on success, -1 on error (errno set).
 *   Flat document only — lines of the form:
 *     key = value
 *   Bare keys: [A-Za-z0-9_-]+
 *   String values: basic double-quoted strings with \\ \" \n \t \r \b \f.
 *   Integer values: optional leading +/- and decimal digits only.
 *   Blank lines, # comments, and [table] headers are skipped.
 *   Nested tables / arrays / multi-line strings are not supported.
 *   errno: EINVAL bad input/syntax, ENOENT missing key, ENOSPC buffer,
 *          ERANGE integer overflow.
 *
 * Soft deepen: has_key probe without value decode; last-key-wins is
 * inherited from find_key scan order.
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch81_marker[] = "libcgj-batch81";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b81_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

static int
b81_is_eol(unsigned char ch)
{
	return ch == '\0' || ch == '\n' || ch == '\r';
}

static int
b81_is_bare_key_ch(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
	       (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
}

static int
b81_key_eq(const char *pKey, size_t cbKey, const char *szKey)
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
b81_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b81_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

/*
 * Skip to start of next line (past \r?\n). Returns pointer at next line
 * or at the terminating NUL.
 */
static const char *
b81_next_line(const char *pSz)
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
 * Decode one basic TOML double-quoted string (opening '"' already
 * consumed). Writes unescaped bytes to pOut if non-NULL.
 * *pcbNeed receives unescaped length (no NUL).
 * On success returns pointer past closing '"'; on error returns NULL.
 * fOverflow is set when pOut is too small (vs bad syntax).
 */
static const char *
b81_parse_string(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed,
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
			/* unsupported escape (incl. \u / \U) */
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
 * Locate value text after "key = " for the first matching bare key.
 * On success: *ppVal points at first non-ws char of the value; return 0.
 * On missing key: errno = ENOENT, return -1.
 * On bad document/key: errno = EINVAL, return -1.
 */
static int
b81_find_key(const char *szToml, const char *szKey, const char **ppVal)
{
	const char *pLine;

	if (szToml == NULL || szKey == NULL || ppVal == NULL ||
	    szKey[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	*ppVal = NULL;
	pLine = szToml;

	while (*pLine != '\0') {
		const char *p;
		const char *pKeyStart;
		size_t cbKey;

		p = b81_skip_ws(pLine);

		/* blank line */
		if (b81_is_eol((unsigned char)*p)) {
			pLine = b81_next_line(p);
			continue;
		}

		/* full-line comment */
		if (*p == '#') {
			pLine = b81_next_line(p);
			continue;
		}

		/* table header — skip (flat key=value only) */
		if (*p == '[') {
			pLine = b81_next_line(p);
			continue;
		}

		/* bare key */
		if (!b81_is_bare_key_ch((unsigned char)*p)) {
			pLine = b81_next_line(p);
			continue;
		}
		pKeyStart = p;
		while (b81_is_bare_key_ch((unsigned char)*p)) {
			p++;
		}
		cbKey = (size_t)(p - pKeyStart);

		p = b81_skip_ws(p);
		if (*p != '=') {
			/* not a key=value assignment line */
			pLine = b81_next_line(p);
			continue;
		}
		p++;
		p = b81_skip_ws(p);

		if (b81_key_eq(pKeyStart, cbKey, szKey)) {
			*ppVal = p;
			return 0;
		}

		pLine = b81_next_line(p);
	}

	errno = ENOENT;
	return -1;
}

/* ---- public: flat key=value getters ------------------------------------ */

/*
 * Extract string value for key from flat key="value" lines.
 * Unescapes into out (NUL-terminated). outcap includes room for NUL.
 */
int
gj_toml_get_string(const char *szToml, const char *szKey, char *szOut,
                   size_t cbOut)
{
	const char *pVal;
	const char *pEnd;
	size_t cbNeed = 0;
	int fOverflow = 0;

	if (szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b81_find_key(szToml, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b81_skip_ws(pVal);
	if (*pVal != '"') {
		errno = EINVAL;
		return -1;
	}
	pVal++;

	pEnd = b81_parse_string(pVal, szOut, cbOut - 1u, &cbNeed, &fOverflow);
	if (pEnd == NULL) {
		if (fOverflow) {
			errno = ENOSPC;
		} else {
			/* distinguish overflow (size-only probe) vs syntax */
			size_t cbProbe = 0;
			int fOv2 = 0;
			const char *pOk =
			    b81_parse_string(pVal, NULL, 0, &cbProbe, &fOv2);

			if (pOk != NULL && cbProbe + 1u > cbOut) {
				errno = ENOSPC;
			} else {
				errno = EINVAL;
			}
		}
		return -1;
	}

	/* trailing: only ws, comment, or EOL */
	pEnd = b81_skip_ws(pEnd);
	if (!b81_is_eol((unsigned char)*pEnd) && *pEnd != '#') {
		errno = EINVAL;
		return -1;
	}

	szOut[cbNeed] = '\0';
	return 0;
}

/*
 * Extract integer value for key from flat key=123 lines.
 * Accepts optional leading +/– and decimal digits only (no fraction/exp).
 */
int
gj_toml_get_int(const char *szToml, const char *szKey, long *pOut)
{
	const char *pVal;
	int fNeg = 0;
	unsigned long uAcc = 0;
	int fDigits = 0;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b81_find_key(szToml, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b81_skip_ws(pVal);
	if (*pVal == '+') {
		pVal++;
	} else if (*pVal == '-') {
		fNeg = 1;
		pVal++;
	}
	if (*pVal < '0' || *pVal > '9') {
		errno = EINVAL;
		return -1;
	}

	while (*pVal >= '0' && *pVal <= '9') {
		unsigned long uDigit = (unsigned long)(*pVal - '0');

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
		pVal++;
	}

	/* reject fraction / exponent / underscore */
	if (*pVal == '.' || *pVal == 'e' || *pVal == 'E' || *pVal == '_') {
		errno = EINVAL;
		return -1;
	}
	if (!fDigits) {
		errno = EINVAL;
		return -1;
	}

	pVal = b81_skip_ws(pVal);
	if (!b81_is_eol((unsigned char)*pVal) && *pVal != '#') {
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

/*
 * gj_toml_has_key — soft deepen: 1 if bare key is assigned, else 0.
 * Does not decode the value. NULL args → 0; restores errno on miss.
 */
int
gj_toml_has_key(const char *szToml, const char *szKey)
{
	const char *pVal;
	int nSaved = errno;

	if (szToml == NULL || szKey == NULL || szKey[0] == '\0') {
		return 0;
	}
	if (b81_find_key(szToml, szKey, &pVal) != 0) {
		errno = nSaved;
		return 0;
	}
	errno = nSaved;
	return 1;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_toml_get_string(const char *szToml, const char *szKey, char *szOut,
                         size_t cbOut)
    __attribute__((alias("gj_toml_get_string")));

int __gj_toml_get_int(const char *szToml, const char *szKey, long *pOut)
    __attribute__((alias("gj_toml_get_int")));

int __gj_toml_has_key(const char *szToml, const char *szKey)
    __attribute__((alias("gj_toml_has_key")));
