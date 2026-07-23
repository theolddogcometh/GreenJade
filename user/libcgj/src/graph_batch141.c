/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch141: flat TOML bool / has-key helpers.
 * Clean-room pure C — integer/pointer only (builds with -msse2).
 * No third-party TOML source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_toml_get_string / gj_toml_get_int  → graph_batch81.c
 *   gj_ini_get / gj_ini_get_int           → graph_batch104.c
 *
 * This TU adds only unique symbols:
 *   int gj_toml_get_bool(const char *toml, const char *key, int *out);
 *   int gj_toml_has_key(const char *toml, const char *key);
 *   __gj_toml_get_bool / __gj_toml_has_key  (aliases)
 *   __libcgj_batch141_marker = "libcgj-batch141"
 *
 * Semantics (flat key=value only, same dialect as batch81):
 *   get_bool: 0 on success, -1 on error (errno set).
 *     *out = 1 for TOML true, 0 for TOML false (lowercase keywords).
 *   has_key: 1 if bare key is present as an assignment, 0 if missing,
 *     -1 on bad arguments (errno = EINVAL). Does not validate value syntax.
 *   Bare keys: [A-Za-z0-9_-]+
 *   Blank lines, # comments, and [table] headers are skipped.
 *   Nested tables / arrays / multi-line values are not supported.
 *   errno: EINVAL bad input/syntax, ENOENT missing key.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch141_marker[] = "libcgj-batch141";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b141_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

static int
b141_is_eol(unsigned char ch)
{
	return ch == '\0' || ch == '\n' || ch == '\r';
}

static int
b141_is_bare_key_ch(unsigned char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
	       (ch >= '0' && ch <= '9') || ch == '_' || ch == '-';
}

static int
b141_key_eq(const char *pKey, size_t cbKey, const char *szKey)
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

static int
b141_streq(const char *p, size_t cb, const char *szLit)
{
	size_t i;

	if (szLit == NULL) {
		return 0;
	}
	for (i = 0; i < cb; i++) {
		if (szLit[i] == '\0' || p[i] != szLit[i]) {
			return 0;
		}
	}
	return szLit[cb] == '\0';
}

static const char *
b141_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b141_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

/*
 * Skip to start of next line (past \r?\n). Returns pointer at next line
 * or at the terminating NUL.
 */
static const char *
b141_next_line(const char *pSz)
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
 * Locate value text after "key = " for the first matching bare key.
 * On success: *ppVal points at first non-ws char of the value; return 0.
 * On missing key: errno = ENOENT, return -1.
 * On bad document/key: errno = EINVAL, return -1.
 */
static int
b141_find_key(const char *szToml, const char *szKey, const char **ppVal)
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

		p = b141_skip_ws(pLine);

		/* blank line */
		if (b141_is_eol((unsigned char)*p)) {
			pLine = b141_next_line(p);
			continue;
		}

		/* full-line comment */
		if (*p == '#') {
			pLine = b141_next_line(p);
			continue;
		}

		/* table header — skip (flat key=value only) */
		if (*p == '[') {
			pLine = b141_next_line(p);
			continue;
		}

		/* bare key */
		if (!b141_is_bare_key_ch((unsigned char)*p)) {
			pLine = b141_next_line(p);
			continue;
		}
		pKeyStart = p;
		while (b141_is_bare_key_ch((unsigned char)*p)) {
			p++;
		}
		cbKey = (size_t)(p - pKeyStart);

		p = b141_skip_ws(p);
		if (*p != '=') {
			/* not a key=value assignment line */
			pLine = b141_next_line(p);
			continue;
		}
		p++;
		p = b141_skip_ws(p);

		if (b141_key_eq(pKeyStart, cbKey, szKey)) {
			*ppVal = p;
			return 0;
		}

		pLine = b141_next_line(p);
	}

	errno = ENOENT;
	return -1;
}

/* ---- public: flat key=value bool / presence ---------------------------- */

/*
 * Extract boolean value for key from flat key=true / key=false lines.
 * TOML booleans are lowercase keywords only. *pOut set to 1 or 0.
 */
int
gj_toml_get_bool(const char *szToml, const char *szKey, int *pOut)
{
	const char *pVal;
	const char *pWord;
	size_t cbWord;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b141_find_key(szToml, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b141_skip_ws(pVal);
	pWord = pVal;

	/* keyword: letters only (true / false) */
	while ((*pVal >= 'a' && *pVal <= 'z') ||
	       (*pVal >= 'A' && *pVal <= 'Z')) {
		pVal++;
	}
	cbWord = (size_t)(pVal - pWord);
	if (cbWord == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* trailing: only ws, comment, or EOL */
	pVal = b141_skip_ws(pVal);
	if (!b141_is_eol((unsigned char)*pVal) && *pVal != '#') {
		errno = EINVAL;
		return -1;
	}

	if (b141_streq(pWord, cbWord, "true")) {
		*pOut = 1;
		return 0;
	}
	if (b141_streq(pWord, cbWord, "false")) {
		*pOut = 0;
		return 0;
	}

	errno = EINVAL;
	return -1;
}

/*
 * Return whether a bare key appears as a flat key=value assignment.
 * 1 = present, 0 = missing, -1 = bad arguments (errno = EINVAL).
 * Value syntax is not validated.
 */
int
gj_toml_has_key(const char *szToml, const char *szKey)
{
	const char *pVal;

	if (szToml == NULL || szKey == NULL || szKey[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	if (b141_find_key(szToml, szKey, &pVal) == 0) {
		return 1;
	}
	if (errno == ENOENT) {
		return 0;
	}
	return -1;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_toml_get_bool(const char *szToml, const char *szKey, int *pOut)
    __attribute__((alias("gj_toml_get_bool")));

int __gj_toml_has_key(const char *szToml, const char *szKey)
    __attribute__((alias("gj_toml_has_key")));
