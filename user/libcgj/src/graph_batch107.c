/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch107: flat JSON bool + array helpers.
 * Extends batch77 (gj_json_get_string / gj_json_get_int) without
 * redefining those symbols. Clean-room freestanding pure C —
 * integer/pointer only (no SSE required). No third-party JSON source
 * was copied.
 *
 * Surface:
 *   gj_json_get_bool(const char *json, const char *key, int *out)
 *     Value must be JSON true/false. *out = 1 or 0. 0 / -1.
 *   gj_json_array_len(const char *json, const char *key)
 *     For "k":[a,b,c] return element count, or -1 on error.
 *   gj_json_array_get_string(const char *json, const char *key,
 *                            unsigned idx, char *out, size_t outcap)
 *     String element at idx from flat array value. 0 / -1.
 *   __gj_json_get_bool / __gj_json_array_len / __gj_json_array_get_string
 *   __libcgj_batch107_marker = "libcgj-batch107"
 *
 * Semantics:
 *   Flat object only. Array values are flat lists of strings and
 *   numbers only (no nested objects/arrays, no true/false/null in
 *   arrays). Sibling object values may be string/number/bool/null or
 *   a flat string/number array so keys past arrays remain reachable.
 *   get_bool / array_get_string: 0 success, -1 error (errno set).
 *   array_len: count >= 0, or -1 with errno.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch107_marker[] = "libcgj-batch107";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b107_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
	       ch == '\f' || ch == '\v';
}

static const char *
b107_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b107_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

static int
b107_key_eq(const char *pKey, size_t cbKey, const char *szKey)
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
b107_hexval(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a' + 10);
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A' + 10);
	}
	return -1;
}

/*
 * Decode one JSON string (opening '"' already consumed). Writes unescaped
 * bytes to pOut if non-NULL. *pcbNeed receives unescaped length (no NUL).
 * On success returns pointer past closing '"'; on error returns NULL.
 */
static const char *
b107_parse_string(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed)
{
	size_t cb = 0;

	if (pSz == NULL) {
		return NULL;
	}

	while (*pSz != '\0' && *pSz != '"') {
		unsigned char ch = (unsigned char)*pSz;
		unsigned char chOut;
		unsigned nUtf;
		unsigned char aUtf[4];
		unsigned iUtf;

		if (ch < 0x20u) {
			return NULL;
		}

		if (ch != (unsigned char)'\\') {
			if (pOut != NULL) {
				if (cb >= cbCap) {
					return NULL;
				}
				pOut[cb] = (char)ch;
			}
			cb++;
			pSz++;
			continue;
		}

		pSz++;
		if (*pSz == '\0') {
			return NULL;
		}
		ch = (unsigned char)*pSz;
		pSz++;

		switch (ch) {
		case '"':
		case '\\':
		case '/':
			chOut = ch;
			nUtf = 1u;
			aUtf[0] = chOut;
			break;
		case 'b':
			nUtf = 1u;
			aUtf[0] = '\b';
			break;
		case 'f':
			nUtf = 1u;
			aUtf[0] = '\f';
			break;
		case 'n':
			nUtf = 1u;
			aUtf[0] = '\n';
			break;
		case 'r':
			nUtf = 1u;
			aUtf[0] = '\r';
			break;
		case 't':
			nUtf = 1u;
			aUtf[0] = '\t';
			break;
		case 'u': {
			int n0, n1, n2, n3;
			unsigned uCp;

			if (pSz[0] == '\0' || pSz[1] == '\0' || pSz[2] == '\0' ||
			    pSz[3] == '\0') {
				return NULL;
			}
			n0 = b107_hexval((unsigned char)pSz[0]);
			n1 = b107_hexval((unsigned char)pSz[1]);
			n2 = b107_hexval((unsigned char)pSz[2]);
			n3 = b107_hexval((unsigned char)pSz[3]);
			if (n0 < 0 || n1 < 0 || n2 < 0 || n3 < 0) {
				return NULL;
			}
			uCp = ((unsigned)n0 << 12) | ((unsigned)n1 << 8) |
			      ((unsigned)n2 << 4) | (unsigned)n3;
			pSz += 4;

			if (uCp >= 0xD800u && uCp <= 0xDFFFu) {
				return NULL;
			}
			if (uCp <= 0x7Fu) {
				nUtf = 1u;
				aUtf[0] = (unsigned char)uCp;
			} else if (uCp <= 0x7FFu) {
				nUtf = 2u;
				aUtf[0] = (unsigned char)(0xC0u | (uCp >> 6));
				aUtf[1] = (unsigned char)(0x80u | (uCp & 0x3Fu));
			} else {
				nUtf = 3u;
				aUtf[0] = (unsigned char)(0xE0u | (uCp >> 12));
				aUtf[1] =
				    (unsigned char)(0x80u | ((uCp >> 6) & 0x3Fu));
				aUtf[2] = (unsigned char)(0x80u | (uCp & 0x3Fu));
			}
			break;
		}
		default:
			return NULL;
		}

		for (iUtf = 0; iUtf < nUtf; iUtf++) {
			if (pOut != NULL) {
				if (cb >= cbCap) {
					return NULL;
				}
				pOut[cb] = (char)aUtf[iUtf];
			}
			cb++;
		}
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
 * Skip a JSON number: -? digits (.digits)? ([eE][+-]?digits)?
 */
static const char *
b107_skip_number(const char *pSz)
{
	if (pSz == NULL || *pSz == '\0') {
		return NULL;
	}
	if (*pSz == '-') {
		pSz++;
	}
	if (*pSz < '0' || *pSz > '9') {
		return NULL;
	}
	while (*pSz >= '0' && *pSz <= '9') {
		pSz++;
	}
	if (*pSz == '.') {
		pSz++;
		if (*pSz < '0' || *pSz > '9') {
			return NULL;
		}
		while (*pSz >= '0' && *pSz <= '9') {
			pSz++;
		}
	}
	if (*pSz == 'e' || *pSz == 'E') {
		pSz++;
		if (*pSz == '+' || *pSz == '-') {
			pSz++;
		}
		if (*pSz < '0' || *pSz > '9') {
			return NULL;
		}
		while (*pSz >= '0' && *pSz <= '9') {
			pSz++;
		}
	}
	return pSz;
}

/*
 * Skip one flat array element: string or number only.
 */
static const char *
b107_skip_array_elem(const char *pSz)
{
	size_t cbNeed = 0;

	pSz = b107_skip_ws(pSz);
	if (pSz == NULL || *pSz == '\0') {
		return NULL;
	}
	if (*pSz == '"') {
		return b107_parse_string(pSz + 1, NULL, 0, &cbNeed);
	}
	/* number (including leading '-') */
	if (*pSz == '-' || (*pSz >= '0' && *pSz <= '9')) {
		return b107_skip_number(pSz);
	}
	return NULL;
}

/*
 * Skip a flat array "[ ... ]" of strings/numbers. pSz points at '['.
 */
static const char *
b107_skip_flat_array(const char *pSz)
{
	if (pSz == NULL || *pSz != '[') {
		return NULL;
	}
	pSz++;
	pSz = b107_skip_ws(pSz);
	if (pSz == NULL) {
		return NULL;
	}
	if (*pSz == ']') {
		return pSz + 1;
	}
	for (;;) {
		pSz = b107_skip_array_elem(pSz);
		if (pSz == NULL) {
			return NULL;
		}
		pSz = b107_skip_ws(pSz);
		if (*pSz == ',') {
			pSz++;
			continue;
		}
		if (*pSz == ']') {
			return pSz + 1;
		}
		return NULL;
	}
}

/*
 * Skip a flat object value: string / number / true / false / null /
 * flat array of strings/numbers. Nested objects rejected.
 */
static const char *
b107_skip_value(const char *pSz)
{
	size_t cbNeed = 0;

	pSz = b107_skip_ws(pSz);
	if (pSz == NULL || *pSz == '\0') {
		return NULL;
	}

	if (*pSz == '"') {
		return b107_parse_string(pSz + 1, NULL, 0, &cbNeed);
	}
	if (*pSz == '[') {
		return b107_skip_flat_array(pSz);
	}
	if (*pSz == '{') {
		/* nested object not supported */
		return NULL;
	}
	if (pSz[0] == 't' && pSz[1] == 'r' && pSz[2] == 'u' && pSz[3] == 'e') {
		return pSz + 4;
	}
	if (pSz[0] == 'f' && pSz[1] == 'a' && pSz[2] == 'l' && pSz[3] == 's' &&
	    pSz[4] == 'e') {
		return pSz + 5;
	}
	if (pSz[0] == 'n' && pSz[1] == 'u' && pSz[2] == 'l' && pSz[3] == 'l') {
		return pSz + 4;
	}
	if (*pSz == '-' || (*pSz >= '0' && *pSz <= '9')) {
		return b107_skip_number(pSz);
	}
	return NULL;
}

/*
 * Locate key in a flat object. On match, *ppVal points at the value
 * start (after ':', ws-skipped by caller as needed). Returns 0.
 * Not found → -1 ENOENT; malformed → -1 EINVAL.
 */
static int
b107_find_key(const char *szJson, const char *szKey, const char **ppVal)
{
	const char *pSz;
	char aKeyBuf[256];
	size_t cbKey;

	if (szJson == NULL || szKey == NULL || ppVal == NULL) {
		errno = EINVAL;
		return -1;
	}

	pSz = b107_skip_ws(szJson);
	if (*pSz != '{') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	pSz = b107_skip_ws(pSz);
	if (*pSz == '}') {
		errno = ENOENT;
		return -1;
	}

	for (;;) {
		pSz = b107_skip_ws(pSz);
		if (*pSz != '"') {
			errno = EINVAL;
			return -1;
		}
		pSz++;

		cbKey = 0;
		pSz = b107_parse_string(pSz, aKeyBuf, sizeof(aKeyBuf) - 1u, &cbKey);
		if (pSz == NULL) {
			errno = EINVAL;
			return -1;
		}
		if (cbKey >= sizeof(aKeyBuf)) {
			errno = EINVAL;
			return -1;
		}
		aKeyBuf[cbKey] = '\0';

		pSz = b107_skip_ws(pSz);
		if (*pSz != ':') {
			errno = EINVAL;
			return -1;
		}
		pSz++;
		pSz = b107_skip_ws(pSz);

		if (b107_key_eq(aKeyBuf, cbKey, szKey)) {
			*ppVal = pSz;
			return 0;
		}

		pSz = b107_skip_value(pSz);
		if (pSz == NULL) {
			errno = EINVAL;
			return -1;
		}

		pSz = b107_skip_ws(pSz);
		if (*pSz == ',') {
			pSz++;
			continue;
		}
		if (*pSz == '}') {
			errno = ENOENT;
			return -1;
		}
		errno = EINVAL;
		return -1;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * Extract boolean value for key from flat {"k":true,...}.
 * Only JSON true / false accepted (*out = 1 / 0).
 */
int
gj_json_get_bool(const char *szJson, const char *szKey, int *pOut)
{
	const char *pVal;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (b107_find_key(szJson, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b107_skip_ws(pVal);
	if (pVal[0] == 't' && pVal[1] == 'r' && pVal[2] == 'u' &&
	    pVal[3] == 'e') {
		*pOut = 1;
		return 0;
	}
	if (pVal[0] == 'f' && pVal[1] == 'a' && pVal[2] == 'l' &&
	    pVal[3] == 's' && pVal[4] == 'e') {
		*pOut = 0;
		return 0;
	}
	errno = EINVAL;
	return -1;
}

/*
 * Return element count of flat array value for key, or -1 on error.
 * "k":[a,b,c] → 3; "k":[] → 0. Elements must be strings or numbers.
 */
int
gj_json_array_len(const char *szJson, const char *szKey)
{
	const char *pVal;
	int nCount = 0;

	if (b107_find_key(szJson, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b107_skip_ws(pVal);
	if (*pVal != '[') {
		errno = EINVAL;
		return -1;
	}
	pVal++;
	pVal = b107_skip_ws(pVal);
	if (*pVal == ']') {
		return 0;
	}

	for (;;) {
		pVal = b107_skip_array_elem(pVal);
		if (pVal == NULL) {
			errno = EINVAL;
			return -1;
		}
		if (nCount == 0x7fffffff) {
			errno = ERANGE;
			return -1;
		}
		nCount++;

		pVal = b107_skip_ws(pVal);
		if (*pVal == ',') {
			pVal++;
			continue;
		}
		if (*pVal == ']') {
			return nCount;
		}
		errno = EINVAL;
		return -1;
	}
}

/*
 * Extract string element at idx from flat array value for key.
 * Unescapes into out (NUL-terminated). outcap includes room for NUL.
 */
int
gj_json_array_get_string(const char *szJson, const char *szKey, unsigned uIdx,
                         char *szOut, size_t cbOut)
{
	const char *pVal;
	unsigned uCur = 0;
	const char *pEnd;
	size_t cbNeed = 0;

	if (szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	if (b107_find_key(szJson, szKey, &pVal) != 0) {
		return -1;
	}

	pVal = b107_skip_ws(pVal);
	if (*pVal != '[') {
		errno = EINVAL;
		return -1;
	}
	pVal++;
	pVal = b107_skip_ws(pVal);
	if (*pVal == ']') {
		errno = ERANGE;
		return -1;
	}

	for (;;) {
		pVal = b107_skip_ws(pVal);
		if (pVal == NULL || *pVal == '\0') {
			errno = EINVAL;
			return -1;
		}

		if (uCur == uIdx) {
			/* target element must be a string */
			if (*pVal != '"') {
				errno = EINVAL;
				return -1;
			}
			pVal++;
			pEnd = b107_parse_string(pVal, szOut, cbOut - 1u, &cbNeed);
			if (pEnd == NULL) {
				size_t cbProbe = 0;
				const char *pOk =
				    b107_parse_string(pVal, NULL, 0, &cbProbe);

				if (pOk != NULL && cbProbe + 1u > cbOut) {
					errno = ENOSPC;
				} else {
					errno = EINVAL;
				}
				return -1;
			}
			szOut[cbNeed] = '\0';
			return 0;
		}

		/* skip non-target element (string or number) */
		pVal = b107_skip_array_elem(pVal);
		if (pVal == NULL) {
			errno = EINVAL;
			return -1;
		}
		uCur++;

		pVal = b107_skip_ws(pVal);
		if (*pVal == ',') {
			pVal++;
			continue;
		}
		if (*pVal == ']') {
			errno = ERANGE;
			return -1;
		}
		errno = EINVAL;
		return -1;
	}
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_json_get_bool(const char *szJson, const char *szKey, int *pOut)
    __attribute__((alias("gj_json_get_bool")));

int __gj_json_array_len(const char *szJson, const char *szKey)
    __attribute__((alias("gj_json_array_len")));

int __gj_json_array_get_string(const char *szJson, const char *szKey,
                               unsigned uIdx, char *szOut, size_t cbOut)
    __attribute__((alias("gj_json_array_get_string")));
