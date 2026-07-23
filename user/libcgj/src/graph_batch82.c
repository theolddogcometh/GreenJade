/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch82: dotenv-style KEY=VAL file helpers.
 * Clean-room freestanding pure C — integer/pointer only (no SSE).
 * No third-party dotenv source was copied.
 *
 * Soft deepen: last-wins scan, tolerant skip of malformed lines, empty
 * key reject, CR-before-LF strip, underscored aliases.
 *
 * Surface:
 *   gj_env_parse_line(const char *line,
 *                     char *key, size_t keycap,
 *                     char *val, size_t valcap)
 *     Parse one line of a dotenv / KEY=VAL file.
 *     Returns 0 on KEY=VAL success, 1 if blank/comment (skip), -1 on error.
 *   gj_env_get(const char *text, const char *key,
 *              char *out, size_t outcap)
 *     Scan multi-line text; last matching key wins (override style).
 *     Returns 0 on success, -1 on error (errno set).
 *   __gj_env_parse_line / __gj_env_get  (aliases)
 *   __libcgj_batch82_marker = "libcgj-batch82"
 *
 * Line grammar (dotenv-ish):
 *   - Leading/trailing line whitespace ignored.
 *   - Empty lines and '#' comments → skip (return 1).
 *   - Optional "export " prefix (case-sensitive, single space).
 *   - KEY: [A-Za-z_][A-Za-z0-9_]*
 *   - Optional whitespace around '='.
 *   - VAL:
 *       double-quoted: \" \\ \n \r \t \$ and other \X → X
 *       single-quoted: literal (no escapes except \')
 *       unquoted: strip trailing WS; inline " #" starts a comment
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch82_marker[] = "libcgj-batch82";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b82_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' ||
	       ch == '\v';
}

static int
b82_is_key_start(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_';
}

static int
b82_is_key_cont(unsigned char ch)
{
	return b82_is_key_start(ch) || (ch >= '0' && ch <= '9');
}

static size_t
b82_strlen(const char *sz)
{
	size_t cb = 0;

	if (sz == NULL) {
		return 0;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

static int
b82_streq(const char *szA, const char *szB)
{
	size_t i;

	if (szA == NULL || szB == NULL) {
		return 0;
	}
	for (i = 0;; i++) {
		if (szA[i] != szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
	}
}

static const char *
b82_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b82_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

/*
 * Copy up to cbNeed bytes into pOut (capacity cbCap including room for NUL
 * when writing). On success writes NUL and returns 0. ENOSPC if too small.
 */
static int
b82_copy_out(char *pOut, size_t cbCap, const char *pSrc, size_t cbNeed)
{
	size_t i;

	if (pOut == NULL || cbCap == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (cbNeed + 1u > cbCap) {
		errno = ENOSPC;
		return -1;
	}
	for (i = 0; i < cbNeed; i++) {
		pOut[i] = pSrc[i];
	}
	pOut[cbNeed] = '\0';
	return 0;
}

/* ---- public: gj_env_parse_line ----------------------------------------- */

/*
 * Parse one dotenv KEY=VAL line into key/val buffers (NUL-terminated).
 * keycap/valcap include room for the trailing NUL.
 */
int
gj_env_parse_line(const char *szLine, char *szKey, size_t cbKey,
                  char *szVal, size_t cbVal)
{
	const char *pSz;
	const char *pKey;
	size_t cbK = 0;
	size_t cbV = 0;
	char aValTmp[512];
	unsigned char ch;

	if (szLine == NULL || szKey == NULL || szVal == NULL || cbKey == 0u ||
	    cbVal == 0u) {
		errno = EINVAL;
		return -1;
	}

	pSz = b82_skip_ws(szLine);
	if (*pSz == '\0' || *pSz == '#') {
		return 1; /* blank or full-line comment */
	}

	/* Optional "export " prefix (dotenv / shell-env style). */
	if (pSz[0] == 'e' && pSz[1] == 'x' && pSz[2] == 'p' && pSz[3] == 'o' &&
	    pSz[4] == 'r' && pSz[5] == 't' && b82_is_ws((unsigned char)pSz[6])) {
		pSz = b82_skip_ws(pSz + 6);
	}

	if (!b82_is_key_start((unsigned char)*pSz)) {
		errno = EINVAL;
		return -1;
	}

	pKey = pSz;
	while (b82_is_key_cont((unsigned char)*pSz)) {
		pSz++;
		cbK++;
	}
	if (cbK + 1u > cbKey) {
		errno = ENOSPC;
		return -1;
	}

	pSz = b82_skip_ws(pSz);
	if (*pSz != '=') {
		errno = EINVAL;
		return -1;
	}
	pSz++; /* skip '=' */
	pSz = b82_skip_ws(pSz);

	/* ---- value --------------------------------------------------------- */

	if (*pSz == '"') {
		/* Double-quoted value with common escapes. */
		pSz++;
		while (*pSz != '\0' && *pSz != '"') {
			ch = (unsigned char)*pSz;
			if (ch == (unsigned char)'\\') {
				pSz++;
				if (*pSz == '\0') {
					errno = EINVAL;
					return -1;
				}
				ch = (unsigned char)*pSz;
				switch (ch) {
				case 'n':
					ch = '\n';
					break;
				case 'r':
					ch = '\r';
					break;
				case 't':
					ch = '\t';
					break;
				case '"':
				case '\\':
				case '$':
				case '\'':
					/* keep ch as-is */
					break;
				default:
					/* unknown escape → literal char */
					break;
				}
				pSz++;
			} else {
				pSz++;
			}
			if (cbV + 1u >= sizeof(aValTmp)) {
				errno = ENOSPC;
				return -1;
			}
			aValTmp[cbV++] = (char)ch;
		}
		if (*pSz != '"') {
			errno = EINVAL;
			return -1;
		}
		pSz++;
		/* Trailing junk after closing quote (except WS / comment) */
		pSz = b82_skip_ws(pSz);
		if (*pSz != '\0' && *pSz != '#') {
			errno = EINVAL;
			return -1;
		}
	} else if (*pSz == '\'') {
		/* Single-quoted: mostly literal; \' is accepted. */
		pSz++;
		while (*pSz != '\0' && *pSz != '\'') {
			ch = (unsigned char)*pSz;
			if (ch == (unsigned char)'\\' && pSz[1] == '\'') {
				ch = '\'';
				pSz += 2;
			} else {
				pSz++;
			}
			if (cbV + 1u >= sizeof(aValTmp)) {
				errno = ENOSPC;
				return -1;
			}
			aValTmp[cbV++] = (char)ch;
		}
		if (*pSz != '\'') {
			errno = EINVAL;
			return -1;
		}
		pSz++;
		pSz = b82_skip_ws(pSz);
		if (*pSz != '\0' && *pSz != '#') {
			errno = EINVAL;
			return -1;
		}
	} else {
		/* Unquoted: read until end or inline comment (" #"). */
		const char *pStart = pSz;
		const char *pEnd;

		while (*pSz != '\0') {
			if (*pSz == '#' &&
			    (pSz == pStart || b82_is_ws((unsigned char)pSz[-1]))) {
				break;
			}
			pSz++;
		}
		pEnd = pSz;
		/* Trim trailing whitespace of the value. */
		while (pEnd > pStart && b82_is_ws((unsigned char)pEnd[-1])) {
			pEnd--;
		}
		cbV = (size_t)(pEnd - pStart);
		if (cbV + 1u > sizeof(aValTmp)) {
			errno = ENOSPC;
			return -1;
		}
		{
			size_t i;

			for (i = 0; i < cbV; i++) {
				aValTmp[i] = pStart[i];
			}
		}
	}

	if (b82_copy_out(szKey, cbKey, pKey, cbK) != 0) {
		return -1;
	}
	if (b82_copy_out(szVal, cbVal, aValTmp, cbV) != 0) {
		return -1;
	}
	return 0;
}

/* ---- public: gj_env_get ------------------------------------------------ */

/*
 * Scan a multi-line KEY=VAL text blob for szKey. Last matching assignment
 * wins (dotenv override semantics). outcap includes room for NUL.
 */
int
gj_env_get(const char *szText, const char *szKey, char *szOut, size_t cbOut)
{
	const char *pCur;
	char aKey[128];
	char aVal[512];
	char aLine[640];
	int fFound = 0;
	size_t cbLast = 0;
	char aLast[512];

	if (szText == NULL || szKey == NULL || szOut == NULL || cbOut == 0u ||
	    szKey[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	pCur = szText;
	while (*pCur != '\0') {
		size_t cbLine = 0;
		int nRc;

		/* Collect one logical line (strip CR before LF). */
		while (*pCur != '\0' && *pCur != '\n') {
			unsigned char ch = (unsigned char)*pCur;

			pCur++;
			if (ch == '\r') {
				continue;
			}
			if (cbLine + 1u >= sizeof(aLine)) {
				errno = ENOSPC;
				return -1;
			}
			aLine[cbLine++] = (char)ch;
		}
		if (*pCur == '\n') {
			pCur++;
		}
		aLine[cbLine] = '\0';

		nRc = gj_env_parse_line(aLine, aKey, sizeof(aKey), aVal,
		                        sizeof(aVal));
		if (nRc < 0) {
			/* Malformed line: skip (tolerant scan). Clear errno. */
			errno = 0;
			continue;
		}
		if (nRc == 1) {
			continue;
		}
		if (b82_streq(aKey, szKey)) {
			size_t i;
			size_t cb = b82_strlen(aVal);

			if (cb + 1u > sizeof(aLast)) {
				errno = ENOSPC;
				return -1;
			}
			for (i = 0; i < cb; i++) {
				aLast[i] = aVal[i];
			}
			aLast[cb] = '\0';
			cbLast = cb;
			fFound = 1;
		}
	}

	if (!fFound) {
		errno = ENOENT;
		return -1;
	}
	return b82_copy_out(szOut, cbOut, aLast, cbLast);
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_env_parse_line(const char *szLine, char *szKey, size_t cbKey,
                        char *szVal, size_t cbVal)
    __attribute__((alias("gj_env_parse_line")));

int __gj_env_get(const char *szText, const char *szKey, char *szOut,
                 size_t cbOut) __attribute__((alias("gj_env_get")));
