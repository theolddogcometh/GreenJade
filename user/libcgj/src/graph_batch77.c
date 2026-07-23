/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch77: minimal freestanding JSON helpers for
 * flat objects of the form {"k":"v","n":123}. Clean-room pure C —
 * integer/pointer only (no SSE). No third-party JSON source was copied.
 *
 * Surface:
 *   gj_json_get_string(const char *json, const char *key,
 *                      char *out, size_t outcap)
 *   gj_json_get_int(const char *json, const char *key, long *out)
 *   gj_json_has_key(const char *json, const char *key) → 1/0
 *   json_escape / json_unescape
 *   __gj_json_get_string / __gj_json_get_int / __gj_json_has_key
 *   __json_escape / __json_unescape
 *   __libcgj_batch77_marker = "libcgj-batch77"
 *
 * Semantics:
 *   get_string / get_int: 0 on success, -1 on error (errno set).
 *   Flat object only — nested objects/arrays are rejected.
 *   json_escape: C string → JSON string body (no surrounding quotes).
 *     out == NULL → returns bytes needed including trailing NUL.
 *     success     → returns encoded length excluding NUL.
 *   json_unescape: JSON string body (no surrounding quotes) → C string.
 *     inlen is byte length of the escaped body (not requiring NUL).
 *     out == NULL → size needed including NUL; else length excl. NUL.
 *
 * Soft deepen: has_key probe (no value parse), flat-object reject paths
 * already strict on nested braces.
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch77_marker[] = "libcgj-batch77";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b77_is_ws(unsigned char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' ||
           ch == '\v';
}

static const char *
b77_skip_ws(const char *pSz)
{
    if (pSz == NULL) {
        return NULL;
    }
    while (*pSz != '\0' && b77_is_ws((unsigned char)*pSz)) {
        pSz++;
    }
    return pSz;
}

static size_t
b77_strlen(const char *pSz)
{
    size_t cb = 0;

    if (pSz == NULL) {
        return 0;
    }
    while (pSz[cb] != '\0') {
        cb++;
    }
    return cb;
}

static int
b77_key_eq(const char *pKeyJson, size_t cbKey, const char *szKey)
{
    size_t i;

    if (szKey == NULL) {
        return 0;
    }
    for (i = 0; i < cbKey; i++) {
        if (szKey[i] == '\0' || pKeyJson[i] != szKey[i]) {
            return 0;
        }
    }
    return szKey[cbKey] == '\0';
}

/*
 * Hex nibble → value, or -1.
 */
static int
b77_hexval(unsigned char ch)
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
b77_parse_string(const char *pSz, char *pOut, size_t cbCap, size_t *pcbNeed)
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
            /* unescaped control char illegal in JSON strings */
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
            n0 = b77_hexval((unsigned char)pSz[0]);
            n1 = b77_hexval((unsigned char)pSz[1]);
            n2 = b77_hexval((unsigned char)pSz[2]);
            n3 = b77_hexval((unsigned char)pSz[3]);
            if (n0 < 0 || n1 < 0 || n2 < 0 || n3 < 0) {
                return NULL;
            }
            uCp = ((unsigned)n0 << 12) | ((unsigned)n1 << 8) |
                  ((unsigned)n2 << 4) | (unsigned)n3;
            pSz += 4;

            /* Encode BMP code point as UTF-8 (surrogates rejected). */
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
                aUtf[1] = (unsigned char)(0x80u | ((uCp >> 6) & 0x3Fu));
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
b77_skip_value(const char *pSz)
{
    pSz = b77_skip_ws(pSz);
    if (pSz == NULL || *pSz == '\0') {
        return NULL;
    }

    if (*pSz == '"') {
        size_t cbNeed = 0;
        return b77_parse_string(pSz + 1, NULL, 0, &cbNeed);
    }

    if (*pSz == '{' || *pSz == '[') {
        /* nested not supported in flat mode */
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
 * Locate key in a flat object. On match, *ppVal points at the value
 * start (after ':'), and returns 0. Not found → -1 with ENOENT.
 * Malformed → -1 with EINVAL.
 */
static int
b77_find_key(const char *szJson, const char *szKey, const char **ppVal)
{
    const char *pSz;
    char aKeyBuf[256];
    size_t cbKey;

    if (szJson == NULL || szKey == NULL || ppVal == NULL) {
        errno = EINVAL;
        return -1;
    }

    pSz = b77_skip_ws(szJson);
    if (*pSz != '{') {
        errno = EINVAL;
        return -1;
    }
    pSz++;

    pSz = b77_skip_ws(pSz);
    if (*pSz == '}') {
        errno = ENOENT;
        return -1;
    }

    for (;;) {
        pSz = b77_skip_ws(pSz);
        if (*pSz != '"') {
            errno = EINVAL;
            return -1;
        }
        pSz++;

        cbKey = 0;
        pSz = b77_parse_string(pSz, aKeyBuf, sizeof(aKeyBuf) - 1u, &cbKey);
        if (pSz == NULL) {
            errno = EINVAL;
            return -1;
        }
        if (cbKey >= sizeof(aKeyBuf)) {
            errno = EINVAL;
            return -1;
        }
        aKeyBuf[cbKey] = '\0';

        pSz = b77_skip_ws(pSz);
        if (*pSz != ':') {
            errno = EINVAL;
            return -1;
        }
        pSz++;
        pSz = b77_skip_ws(pSz);

        if (b77_key_eq(aKeyBuf, cbKey, szKey)) {
            *ppVal = pSz;
            return 0;
        }

        pSz = b77_skip_value(pSz);
        if (pSz == NULL) {
            errno = EINVAL;
            return -1;
        }

        pSz = b77_skip_ws(pSz);
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

/* ---- public: json_escape / json_unescape ------------------------------- */

/*
 * Escape a C string into a JSON string body (no surrounding quotes).
 */
ssize_t
json_escape(const char *szIn, char *szOut, size_t cbOut)
{
    size_t iIn;
    size_t cbNeed = 0;
    size_t cbIn;

    if (szIn == NULL) {
        errno = EINVAL;
        return (ssize_t)-1;
    }

    cbIn = b77_strlen(szIn);
    for (iIn = 0; iIn < cbIn; iIn++) {
        unsigned char ch = (unsigned char)szIn[iIn];
        size_t cbAdd;

        if (ch == '"' || ch == '\\') {
            cbAdd = 2u;
        } else if (ch == '\b' || ch == '\f' || ch == '\n' || ch == '\r' ||
                   ch == '\t') {
            cbAdd = 2u;
        } else if (ch < 0x20u) {
            cbAdd = 6u; /* \u00XX */
        } else {
            cbAdd = 1u;
        }
        cbNeed += cbAdd;
    }

    if (szOut == NULL) {
        return (ssize_t)(cbNeed + 1u); /* include NUL */
    }
    if (cbOut == 0u || cbNeed + 1u > cbOut) {
        errno = ENOSPC;
        return (ssize_t)-1;
    }

    {
        size_t iOut = 0;

        for (iIn = 0; iIn < cbIn; iIn++) {
            unsigned char ch = (unsigned char)szIn[iIn];
            char chEsc;

            if (ch == '"' || ch == '\\') {
                szOut[iOut++] = '\\';
                szOut[iOut++] = (char)ch;
                continue;
            }
            chEsc = '\0';
            if (ch == '\b') {
                chEsc = 'b';
            } else if (ch == '\f') {
                chEsc = 'f';
            } else if (ch == '\n') {
                chEsc = 'n';
            } else if (ch == '\r') {
                chEsc = 'r';
            } else if (ch == '\t') {
                chEsc = 't';
            }
            if (chEsc != '\0') {
                szOut[iOut++] = '\\';
                szOut[iOut++] = chEsc;
                continue;
            }
            if (ch < 0x20u) {
                static const char aHex[] = "0123456789abcdef";

                szOut[iOut++] = '\\';
                szOut[iOut++] = 'u';
                szOut[iOut++] = '0';
                szOut[iOut++] = '0';
                szOut[iOut++] = aHex[(ch >> 4) & 0xFu];
                szOut[iOut++] = aHex[ch & 0xFu];
                continue;
            }
            szOut[iOut++] = (char)ch;
        }
        szOut[iOut] = '\0';
    }
    return (ssize_t)cbNeed;
}

/*
 * Unescape a JSON string body (no surrounding quotes) into a C string.
 * cbIn is the length of the escaped body (bytes); NUL not required.
 */
ssize_t
json_unescape(const char *szIn, size_t cbIn, char *szOut, size_t cbOut)
{
    size_t iIn = 0;
    size_t cbNeed = 0;

    if (szIn == NULL) {
        errno = EINVAL;
        return (ssize_t)-1;
    }

    /* First pass: compute length / validate. */
    while (iIn < cbIn) {
        unsigned char ch = (unsigned char)szIn[iIn];

        if (ch < 0x20u) {
            errno = EINVAL;
            return (ssize_t)-1;
        }
        if (ch != (unsigned char)'\\') {
            cbNeed++;
            iIn++;
            continue;
        }
        iIn++;
        if (iIn >= cbIn) {
            errno = EINVAL;
            return (ssize_t)-1;
        }
        ch = (unsigned char)szIn[iIn];
        iIn++;
        switch (ch) {
        case '"':
        case '\\':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
            cbNeed++;
            break;
        case 'u': {
            unsigned uCp;
            int n0, n1, n2, n3;

            if (iIn + 4u > cbIn) {
                errno = EINVAL;
                return (ssize_t)-1;
            }
            n0 = b77_hexval((unsigned char)szIn[iIn]);
            n1 = b77_hexval((unsigned char)szIn[iIn + 1u]);
            n2 = b77_hexval((unsigned char)szIn[iIn + 2u]);
            n3 = b77_hexval((unsigned char)szIn[iIn + 3u]);
            if (n0 < 0 || n1 < 0 || n2 < 0 || n3 < 0) {
                errno = EINVAL;
                return (ssize_t)-1;
            }
            uCp = ((unsigned)n0 << 12) | ((unsigned)n1 << 8) |
                  ((unsigned)n2 << 4) | (unsigned)n3;
            iIn += 4u;
            if (uCp >= 0xD800u && uCp <= 0xDFFFu) {
                errno = EINVAL;
                return (ssize_t)-1;
            }
            if (uCp <= 0x7Fu) {
                cbNeed += 1u;
            } else if (uCp <= 0x7FFu) {
                cbNeed += 2u;
            } else {
                cbNeed += 3u;
            }
            break;
        }
        default:
            errno = EINVAL;
            return (ssize_t)-1;
        }
    }

    if (szOut == NULL) {
        return (ssize_t)(cbNeed + 1u);
    }
    if (cbOut == 0u || cbNeed + 1u > cbOut) {
        errno = ENOSPC;
        return (ssize_t)-1;
    }

    /* Second pass: write. */
    iIn = 0;
    {
        size_t iOut = 0;

        while (iIn < cbIn) {
            unsigned char ch = (unsigned char)szIn[iIn];

            if (ch != (unsigned char)'\\') {
                szOut[iOut++] = (char)ch;
                iIn++;
                continue;
            }
            iIn++;
            ch = (unsigned char)szIn[iIn];
            iIn++;
            switch (ch) {
            case '"':
            case '\\':
            case '/':
                szOut[iOut++] = (char)ch;
                break;
            case 'b':
                szOut[iOut++] = '\b';
                break;
            case 'f':
                szOut[iOut++] = '\f';
                break;
            case 'n':
                szOut[iOut++] = '\n';
                break;
            case 'r':
                szOut[iOut++] = '\r';
                break;
            case 't':
                szOut[iOut++] = '\t';
                break;
            case 'u': {
                int n0 = b77_hexval((unsigned char)szIn[iIn]);
                int n1 = b77_hexval((unsigned char)szIn[iIn + 1u]);
                int n2 = b77_hexval((unsigned char)szIn[iIn + 2u]);
                int n3 = b77_hexval((unsigned char)szIn[iIn + 3u]);
                unsigned uCp = ((unsigned)n0 << 12) | ((unsigned)n1 << 8) |
                               ((unsigned)n2 << 4) | (unsigned)n3;

                iIn += 4u;
                if (uCp <= 0x7Fu) {
                    szOut[iOut++] = (char)uCp;
                } else if (uCp <= 0x7FFu) {
                    szOut[iOut++] = (char)(0xC0u | (uCp >> 6));
                    szOut[iOut++] = (char)(0x80u | (uCp & 0x3Fu));
                } else {
                    szOut[iOut++] = (char)(0xE0u | (uCp >> 12));
                    szOut[iOut++] =
                        (char)(0x80u | ((uCp >> 6) & 0x3Fu));
                    szOut[iOut++] = (char)(0x80u | (uCp & 0x3Fu));
                }
                break;
            }
            default:
                errno = EINVAL;
                return (ssize_t)-1;
            }
        }
        szOut[iOut] = '\0';
    }
    return (ssize_t)cbNeed;
}

/* ---- public: flat-object getters --------------------------------------- */

/*
 * Extract string value for key from flat {"k":"v",...}.
 * Unescapes into out (NUL-terminated). outcap includes room for NUL.
 */
int
gj_json_get_string(const char *szJson, const char *szKey, char *szOut,
                   size_t cbOut)
{
    const char *pVal;
    const char *pEnd;
    size_t cbNeed = 0;

    if (szOut == NULL || cbOut == 0u) {
        errno = EINVAL;
        return -1;
    }

    if (b77_find_key(szJson, szKey, &pVal) != 0) {
        return -1;
    }

    pVal = b77_skip_ws(pVal);
    if (*pVal != '"') {
        errno = EINVAL;
        return -1;
    }
    pVal++;

    pEnd = b77_parse_string(pVal, szOut, cbOut - 1u, &cbNeed);
    if (pEnd == NULL) {
        /* distinguish overflow vs bad syntax: try size-only */
        size_t cbProbe = 0;
        const char *pOk = b77_parse_string(pVal, NULL, 0, &cbProbe);

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

/*
 * Extract integer value for key from flat {"k":123,...}.
 * Accepts optional leading minus and decimal digits only (no fraction/exp).
 */
int
gj_json_get_int(const char *szJson, const char *szKey, long *pOut)
{
    const char *pVal;
    int fNeg = 0;
    unsigned long uAcc = 0;
    int fDigits = 0;

    if (pOut == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (b77_find_key(szJson, szKey, &pVal) != 0) {
        return -1;
    }

    pVal = b77_skip_ws(pVal);
    if (*pVal == '-') {
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

    /* reject fraction / exponent for get_int (strict integer) */
    if (*pVal == '.' || *pVal == 'e' || *pVal == 'E') {
        errno = EINVAL;
        return -1;
    }
    if (!fDigits) {
        errno = EINVAL;
        return -1;
    }

    if (fNeg) {
        /* long min: -LONG_MAX-1; avoid signed overflow */
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
 * gj_json_has_key — soft deepen: 1 if key exists in flat object, else 0.
 * Does not parse the value. NULL json/key → 0. Does not set errno on miss.
 */
int
gj_json_has_key(const char *szJson, const char *szKey)
{
	const char *pVal;
	int nSaved = errno;

	if (szJson == NULL || szKey == NULL || szKey[0] == '\0') {
		return 0;
	}
	if (b77_find_key(szJson, szKey, &pVal) != 0) {
		errno = nSaved;
		return 0;
	}
	errno = nSaved;
	return 1;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_json_get_string(const char *szJson, const char *szKey, char *szOut,
                         size_t cbOut)
    __attribute__((alias("gj_json_get_string")));

int __gj_json_get_int(const char *szJson, const char *szKey, long *pOut)
    __attribute__((alias("gj_json_get_int")));

int __gj_json_has_key(const char *szJson, const char *szKey)
    __attribute__((alias("gj_json_has_key")));

ssize_t __json_escape(const char *szIn, char *szOut, size_t cbOut)
    __attribute__((alias("json_escape")));

ssize_t __json_unescape(const char *szIn, size_t cbIn, char *szOut,
                        size_t cbOut)
    __attribute__((alias("json_unescape")));
