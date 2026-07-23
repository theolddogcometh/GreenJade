/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch161: JSON Pointer (RFC 6901) lite for flat keys.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party JSON source was copied.
 *
 * Surface:
 *   int gj_json_pointer_get(const char *json, const char *ptr,
 *                           char *out, size_t cap);
 *     — ptr is a single-token pointer like "/foo" (flat object key only).
 *       RFC 6901 escapes: "~1" → '/', "~0" → '~'. Nested "/a/b" rejected.
 *       Writes the selected flat scalar value into out (NUL-terminated).
 *       String values are unescaped; numbers/true/false/null are copied
 *       as their JSON literal text. Returns 0 on success, -1 on error.
 *   __gj_json_pointer_get  (alias)
 *   __libcgj_batch161_marker = "libcgj-batch161"
 *
 * Flat object scan matches the batch77 shape: {"k":"v","n":123} only —
 * nested objects/arrays as values are rejected. errno set on failure
 * (EINVAL / ENOENT / ENOSPC) when the hosted errno path is available.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch161_marker[] = "libcgj-batch161";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b161_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
	       ch == '\f' || ch == '\v';
}

static const char *
b161_skip_ws(const char *pSz)
{
	if (pSz == NULL) {
		return NULL;
	}
	while (*pSz != '\0' && b161_is_ws((unsigned char)*pSz)) {
		pSz++;
	}
	return pSz;
}

static int
b161_hexval(unsigned char ch)
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

static int
b161_key_eq(const char *pKeyJson, size_t cbKey, const char *szKey,
	    size_t cbWant)
{
	size_t i;

	if (cbKey != cbWant) {
		return 0;
	}
	for (i = 0; i < cbKey; i++) {
		if (pKeyJson[i] != szKey[i]) {
			return 0;
		}
	}
	return 1;
}

/*
 * Decode one JSON string (opening '"' already consumed). Writes unescaped
 * bytes to pOut if non-NULL. *pcbNeed receives unescaped length (no NUL).
 * On success returns pointer past closing '"'; on error returns NULL.
 */
static const char *
b161_parse_string(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed)
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
			n0 = b161_hexval((unsigned char)pSz[0]);
			n1 = b161_hexval((unsigned char)pSz[1]);
			n2 = b161_hexval((unsigned char)pSz[2]);
			n3 = b161_hexval((unsigned char)pSz[3]);
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
 * Skip a flat JSON value (string / number / true / false / null).
 * Nested object/array → fail. Returns pointer past value, or NULL.
 */
static const char *
b161_skip_value(const char *pSz)
{
	pSz = b161_skip_ws(pSz);
	if (pSz == NULL || *pSz == '\0') {
		return NULL;
	}

	if (*pSz == '"') {
		size_t cbNeed = 0;

		return b161_parse_string(pSz + 1, NULL, 0, &cbNeed);
	}

	if (*pSz == '{' || *pSz == '[') {
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

	/* number: -? [0-9]+ (.[0-9]+)? ([eE][+-]?[0-9]+)? */
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
 * Locate key (exact bytes, length cbKey) in a flat object. On match,
 * *ppVal points at the value start (after ':'); returns 0.
 * Not found → -1 ENOENT. Malformed → -1 EINVAL.
 */
static int
b161_find_key(const char *szJson, const char *szKey, size_t cbKey,
	      const char **ppVal)
{
	const char *pSz;
	char aKeyBuf[256];
	size_t cbParsed;

	if (szJson == NULL || szKey == NULL || ppVal == NULL) {
		errno = EINVAL;
		return -1;
	}

	pSz = b161_skip_ws(szJson);
	if (*pSz != '{') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	pSz = b161_skip_ws(pSz);
	if (*pSz == '}') {
		errno = ENOENT;
		return -1;
	}

	for (;;) {
		pSz = b161_skip_ws(pSz);
		if (*pSz != '"') {
			errno = EINVAL;
			return -1;
		}
		pSz++;

		cbParsed = 0;
		pSz = b161_parse_string(pSz, aKeyBuf, sizeof(aKeyBuf) - 1u,
					&cbParsed);
		if (pSz == NULL) {
			errno = EINVAL;
			return -1;
		}
		if (cbParsed >= sizeof(aKeyBuf)) {
			errno = EINVAL;
			return -1;
		}
		aKeyBuf[cbParsed] = '\0';

		pSz = b161_skip_ws(pSz);
		if (*pSz != ':') {
			errno = EINVAL;
			return -1;
		}
		pSz++;
		pSz = b161_skip_ws(pSz);

		if (b161_key_eq(aKeyBuf, cbParsed, szKey, cbKey)) {
			*ppVal = pSz;
			return 0;
		}

		pSz = b161_skip_value(pSz);
		if (pSz == NULL) {
			errno = EINVAL;
			return -1;
		}

		pSz = b161_skip_ws(pSz);
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

/*
 * Decode a single RFC 6901 reference token (no leading '/'). Writes into
 * pOut up to cbCap bytes (no NUL). *pcbOut = decoded length.
 * Rejects bare '~' not followed by 0/1. Returns 0 / -1.
 */
static int
b161_decode_token(const char *szTok, size_t cbTok, char *pOut, size_t cbCap,
		  size_t *pcbOut)
{
	size_t iIn = 0;
	size_t cb = 0;

	if (szTok == NULL || pOut == NULL || pcbOut == NULL) {
		return -1;
	}

	while (iIn < cbTok) {
		unsigned char ch = (unsigned char)szTok[iIn];

		if (ch == (unsigned char)'~') {
			if (iIn + 1u >= cbTok) {
				return -1;
			}
			if (szTok[iIn + 1u] == '0') {
				ch = (unsigned char)'~';
				iIn += 2u;
			} else if (szTok[iIn + 1u] == '1') {
				ch = (unsigned char)'/';
				iIn += 2u;
			} else {
				return -1;
			}
		} else {
			iIn++;
		}

		if (cb >= cbCap) {
			return -1;
		}
		pOut[cb] = (char)ch;
		cb++;
	}

	*pcbOut = cb;
	return 0;
}

/*
 * Copy a non-string scalar literal (number / true / false / null) into out.
 * pSz is at the first non-ws value char. Returns 0 / -1 (errno set).
 */
static int
b161_copy_literal(const char *pSz, char *szOut, size_t cbOut)
{
	const char *pEnd;
	size_t cb;
	size_t i;

	pEnd = b161_skip_value(pSz);
	if (pEnd == NULL) {
		errno = EINVAL;
		return -1;
	}
	cb = (size_t)(pEnd - pSz);
	if (cb + 1u > cbOut) {
		errno = ENOSPC;
		return -1;
	}
	for (i = 0; i < cb; i++) {
		szOut[i] = pSz[i];
	}
	szOut[cb] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_pointer_get — RFC 6901 lite, flat single-token pointers only.
 *
 * ptr form: "/token" where token may contain "~0" / "~1" escapes.
 * Multi-segment pointers (extra '/') are rejected. Value is written as:
 *   string  → unescaped text
 *   other   → JSON literal text (digits / true / false / null)
 */
int
gj_json_pointer_get(const char *szJson, const char *szPtr, char *szOut,
		    size_t cbOut)
{
	const char *pTok;
	const char *pSlash;
	const char *pVal;
	char aKey[256];
	size_t cbTok;
	size_t cbKey = 0;
	size_t cbNeed = 0;
	const char *pEnd;

	if (szJson == NULL || szPtr == NULL || szOut == NULL || cbOut == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* Flat keys only: require leading '/' and no further unescaped '/'. */
	if (szPtr[0] != '/') {
		errno = EINVAL;
		return -1;
	}
	pTok = szPtr + 1;
	pSlash = pTok;
	while (*pSlash != '\0') {
		if (*pSlash == '/') {
			/* multi-segment — nested path not supported */
			errno = EINVAL;
			return -1;
		}
		pSlash++;
	}
	cbTok = (size_t)(pSlash - pTok);

	if (b161_decode_token(pTok, cbTok, aKey, sizeof(aKey), &cbKey) != 0) {
		errno = EINVAL;
		return -1;
	}

	if (b161_find_key(szJson, aKey, cbKey, &pVal) != 0) {
		return -1;
	}

	pVal = b161_skip_ws(pVal);
	if (*pVal == '"') {
		pVal++;
		pEnd = b161_parse_string(pVal, szOut, cbOut - 1u, &cbNeed);
		if (pEnd == NULL) {
			size_t cbProbe = 0;
			const char *pOk =
			    b161_parse_string(pVal, NULL, 0, &cbProbe);

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

	/* number / true / false / null */
	return b161_copy_literal(pVal, szOut, cbOut);
}

int __gj_json_pointer_get(const char *szJson, const char *szPtr, char *szOut,
			  size_t cbOut)
    __attribute__((alias("gj_json_pointer_get")));
