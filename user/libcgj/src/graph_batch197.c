/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch197: application/x-www-form-urlencoded get.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party form source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   gj_url_query_get  → graph_batch167.c  (raw query, no decode)
 *   gj_url_decode     → graph_batch103.c  (percent only; '+' left alone)
 *
 * This TU adds only unique symbols:
 *   int gj_form_get(const char *body, const char *key,
 *                   char *out, size_t cap);
 *     — Look up the first occurrence of key in an
 *       application/x-www-form-urlencoded body (e.g. POST body):
 *         "a=1&b=two+words&c=a%26b&flag"
 *       Pairs are separated by '&'. Within a pair, key is everything
 *       before the first '=', value is everything after (may be empty).
 *       A pair with no '=' is a bare key with empty value.
 *       Key matching compares the form-decoded key slice to key
 *       (NUL-terminated plain name). Value is form-decoded into out:
 *         '+' → space, %HH → octet (hex case-insensitive).
 *       Invalid or truncated % sequences fail the lookup (-1).
 *       out receives a NUL-terminated decoded value when found.
 *       Returns 0 on success, -1 on error (NULL args, empty key,
 *       not found, bad encoding, or value does not fit including NUL).
 *   __gj_form_get  (alias)
 *   __libcgj_batch197_marker = "libcgj-batch197"
 *
 * Notes:
 *   - First matching key wins if the key is repeated.
 *   - No errno (freestanding-safe); callers use return codes only.
 *   - Compiles with -ffreestanding -msse2.
 */

#include <stddef.h>

const char __libcgj_batch197_marker[] = "libcgj-batch197";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b197_hex_val(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	return -1;
}

/*
 * Form-decode one octet from *ppIn (advancing past consumed input).
 * pEnd is one past the last byte of the slice.
 * Returns 0 and stores the decoded byte in *pOut on success; -1 on
 * truncated/invalid percent-encoding.
 */
static int
b197_decode_one(const char **ppIn, const char *pEnd, unsigned char *pOut)
{
	const char *p;
	unsigned char ch;
	int nHi;
	int nLo;

	p = *ppIn;
	if (p >= pEnd) {
		return -1;
	}
	ch = (unsigned char)*p;
	if (ch == '+') {
		*pOut = (unsigned char)' ';
		*ppIn = p + 1;
		return 0;
	}
	if (ch == '%') {
		if (p + 2 >= pEnd) {
			return -1;
		}
		nHi = b197_hex_val((unsigned char)p[1]);
		nLo = b197_hex_val((unsigned char)p[2]);
		if (nHi < 0 || nLo < 0) {
			return -1;
		}
		*pOut = (unsigned char)((nHi << 4) | nLo);
		*ppIn = p + 3;
		return 0;
	}
	*pOut = ch;
	*ppIn = p + 1;
	return 0;
}

/*
 * True if the form-encoded key slice [pKey, pKeyEnd) decodes equal to
 * szKey (NUL-terminated plain name). Streaming compare — no temp buffer.
 */
static int
b197_key_eq_decoded(const char *pKey, const char *pKeyEnd, const char *szKey)
{
	const char *p;
	size_t i;
	unsigned char ch;

	if (pKey == NULL || szKey == NULL) {
		return 0;
	}

	p = pKey;
	i = 0u;
	while (p < pKeyEnd) {
		if (b197_decode_one(&p, pKeyEnd, &ch) != 0) {
			return 0;
		}
		if (szKey[i] == '\0' || (unsigned char)szKey[i] != ch) {
			return 0;
		}
		i++;
	}
	return (szKey[i] == '\0') ? 1 : 0;
}

/*
 * Form-decode [pSrc, pSrcEnd) into szOut (cap bytes including NUL).
 * Returns 0 on success, -1 if decode fails or result does not fit.
 */
static int
b197_decode_out(char *szOut, size_t cbCap, const char *pSrc,
    const char *pSrcEnd)
{
	const char *p;
	size_t iOut;
	unsigned char ch;

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}

	p = pSrc;
	iOut = 0u;
	while (p < pSrcEnd) {
		if (b197_decode_one(&p, pSrcEnd, &ch) != 0) {
			return -1;
		}
		/* Need room for this byte plus trailing NUL. */
		if (iOut + 1u >= cbCap) {
			return -1;
		}
		szOut[iOut++] = (char)ch;
	}
	szOut[iOut] = '\0';
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_form_get — extract one form-urlencoded field value by key.
 *
 * body is the raw application/x-www-form-urlencoded payload (no leading
 * '?'). Walks &-separated pairs. On match, writes the form-decoded value
 * (possibly empty) into out[0..cap).
 */
int
gj_form_get(const char *body, const char *key, char *out, size_t cap)
{
	const char *p;
	const char *pPair;
	const char *pEq;
	const char *pEnd;

	if (body == NULL || key == NULL || out == NULL || cap == 0u) {
		return -1;
	}
	if (key[0] == '\0') {
		return -1;
	}

	p = body;
	while (*p != '\0') {
		/* Skip empty pairs ("&&" or leading/trailing '&'). */
		if (*p == '&') {
			p++;
			continue;
		}

		pPair = p;
		/* Pair extends to next '&' or NUL. */
		while (*p != '\0' && *p != '&') {
			p++;
		}
		pEnd = p;

		/* Locate first '=' within this pair (if any). */
		pEq = pPair;
		while (pEq < pEnd && *pEq != '=') {
			pEq++;
		}

		if (pEq < pEnd && *pEq == '=') {
			if (b197_key_eq_decoded(pPair, pEq, key)) {
				return b197_decode_out(out, cap, pEq + 1,
				    pEnd);
			}
		} else {
			/* Bare key, empty value. */
			if (b197_key_eq_decoded(pPair, pEnd, key)) {
				if (cap == 0u) {
					return -1;
				}
				out[0] = '\0';
				return 0;
			}
		}

		/* p is at '&' or NUL — loop advances past '&'. */
	}

	return -1;
}

int __gj_form_get(const char *body, const char *key, char *out, size_t cap)
    __attribute__((alias("gj_form_get")));
