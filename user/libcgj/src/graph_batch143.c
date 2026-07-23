/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch143: MIME encoded-word B (base64) lite.
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party MIME source was copied.
 *
 * RFC 2047 encoded-word shape (B encoding only in this TU):
 *   =?charset?B?encoded_text?=
 *   Example: =?UTF-8?B?SGVsbG8=?=  →  "Hello"
 *
 * Surface (unique symbols):
 *   int gj_mime_b64_word_decode(const char *in, char *out, size_t outcap,
 *                               size_t *olen);
 *     Parse one =?…?B?…?= word at in. Charset token is accepted but not
 *     transcoded (octets are emitted as base64-decoded). out may be NULL
 *     to query size via *olen (same idea as base64_decode). On success
 *     *olen is the decoded byte count; if out != NULL and outcap > *olen
 *     a trailing NUL is also written. Returns 0 / -1.
 *   int gj_mime_b64_word_encode(const void *in, size_t inlen, char *out,
 *                               size_t outcap, size_t *olen);
 *     Emit =?UTF-8?B?<base64>?= for in[0..inlen). out may be NULL to
 *     query the full word length (including the terminating NUL that
 *     encode always writes when out is non-NULL). *olen is bytes written
 *     excluding the trailing NUL. Returns 0 / -1.
 *   __gj_mime_b64_word_decode / __gj_mime_b64_word_encode  (aliases)
 *   __libcgj_batch143_marker = "libcgj-batch143"
 *
 * base64_encode / base64_decode live in graph_batch40.c — declared
 * extern here (no multi-def). Encoding letter is case-insensitive B.
 * Lite: no Q-encoding, no multi-word header folding, no charset convert.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h> /* ssize_t for base64_{en,de}code */

const char __libcgj_batch143_marker[] = "libcgj-batch143";

/* ---- extern: base64 from graph_batch40 (do not redefine) --------------- */

extern ssize_t base64_decode(const char *szIn, size_t cbIn,
                             unsigned char *pOut, size_t cbOut);
extern ssize_t base64_encode(const unsigned char *pIn, size_t cbIn,
                             char *szOut, size_t cbOut);

/* Fixed encode wrapper: "=?UTF-8?B?" (10) + b64 + "?=" (2). */
#define B143_PREFIX "=?UTF-8?B?"
#define B143_PREFIX_LEN 10u
#define B143_SUFFIX_LEN 2u /* "?=" */

/* ---- freestanding helpers ---------------------------------------------- */

static int
b143_is_token_char(unsigned char c)
{
	/* RFC 2047 charset/token: printable ASCII except SPACE and specials.
	 * Lite: reject SPACE, '?', control, and DEL; accept the rest. */
	if (c <= 0x20u || c >= 0x7fu) {
		return 0;
	}
	if (c == (unsigned char)'?') {
		return 0;
	}
	return 1;
}

/* ---- public: decode ---------------------------------------------------- */

/*
 * Decode a single RFC 2047 B-encoded-word at in into out.
 * Does not require a trailing NUL after "?=" (stops at "?="), but in must
 * be a C string so the scanner can bound itself with '\0'.
 */
int
gj_mime_b64_word_decode(const char *in, char *out, size_t outcap, size_t *olen)
{
	const char *p;
	const char *pCharset;
	const char *pEncText;
	size_t cbEnc;
	ssize_t nDec;
	unsigned char *pOutBin;

	if (in == NULL || olen == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (out == NULL && outcap != 0u) {
		errno = EINVAL;
		return -1;
	}

	p = in;

	/* "=?" */
	if (p[0] != '=' || p[1] != '?') {
		errno = EINVAL;
		return -1;
	}
	p += 2;

	/* charset token (non-empty) */
	pCharset = p;
	while (*p != '\0' && b143_is_token_char((unsigned char)*p)) {
		p++;
	}
	if (p == pCharset || *p != '?') {
		errno = EINVAL;
		return -1;
	}
	p++; /* skip '?' after charset */

	/* encoding must be B / b */
	if (*p != 'B' && *p != 'b') {
		errno = EINVAL;
		return -1;
	}
	p++;
	if (*p != '?') {
		errno = EINVAL;
		return -1;
	}
	p++; /* skip '?' after encoding */

	/* encoded_text runs until "?=" (no bare '?') */
	pEncText = p;
	while (*p != '\0') {
		if (p[0] == '?' && p[1] == '=') {
			break;
		}
		if (*p == '?') {
			/* bare '?' is illegal inside encoded-text */
			errno = EINVAL;
			return -1;
		}
		p++;
	}
	if (p[0] != '?' || p[1] != '=') {
		errno = EINVAL;
		return -1;
	}
	cbEnc = (size_t)(p - pEncText);

	/* Decode payload via batch40 base64. out==NULL is size-query. */
	pOutBin = (unsigned char *)(void *)out;
	nDec = base64_decode(pEncText, cbEnc, pOutBin, outcap);
	if (nDec < 0) {
		return -1;
	}

	*olen = (size_t)nDec;
	/* Optional trailing NUL when the caller left room (text-friendly). */
	if (out != NULL && outcap > (size_t)nDec) {
		out[nDec] = '\0';
	}
	return 0;
}

/* ---- public: encode ---------------------------------------------------- */

/*
 * Build =?UTF-8?B?<base64(in)>?= into out.
 * in may be NULL only when inlen == 0. *olen excludes trailing NUL.
 */
int
gj_mime_b64_word_encode(const void *in, size_t inlen, char *out,
                        size_t outcap, size_t *olen)
{
	const unsigned char *pIn;
	ssize_t nB64Need;
	size_t cbB64;
	size_t cbWord;
	size_t cbNeed;
	size_t i;
	ssize_t nB64;

	if (olen == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (in == NULL && inlen != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (out == NULL && outcap != 0u) {
		errno = EINVAL;
		return -1;
	}

	pIn = (const unsigned char *)in;

	/* base64_encode(NULL out) returns length including trailing NUL. */
	nB64Need = base64_encode(pIn, inlen, NULL, 0);
	if (nB64Need < 1) {
		if (nB64Need < 0) {
			return -1;
		}
		errno = EINVAL;
		return -1;
	}
	cbB64 = (size_t)nB64Need - 1u; /* exclude base64's own NUL */

	/* Guard: prefix + b64 + suffix + NUL */
	if (cbB64 > SIZE_MAX - (B143_PREFIX_LEN + B143_SUFFIX_LEN + 1u)) {
		errno = EOVERFLOW;
		return -1;
	}
	cbWord = B143_PREFIX_LEN + cbB64 + B143_SUFFIX_LEN;
	cbNeed = cbWord + 1u; /* with NUL */

	if (out == NULL) {
		*olen = cbWord;
		return 0;
	}
	if (outcap < cbNeed) {
		errno = ENOSPC;
		return -1;
	}

	/* prefix */
	for (i = 0u; i < B143_PREFIX_LEN; i++) {
		out[i] = B143_PREFIX[i];
	}

	nB64 = base64_encode(pIn, inlen, out + B143_PREFIX_LEN,
	                     outcap - B143_PREFIX_LEN);
	if (nB64 < 0) {
		return -1;
	}
	if ((size_t)nB64 != cbB64) {
		errno = EINVAL;
		return -1;
	}

	out[B143_PREFIX_LEN + cbB64] = '?';
	out[B143_PREFIX_LEN + cbB64 + 1u] = '=';
	out[cbWord] = '\0';
	*olen = cbWord;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_mime_b64_word_decode(const char *in, char *out, size_t outcap,
                              size_t *olen)
    __attribute__((alias("gj_mime_b64_word_decode")));
int __gj_mime_b64_word_encode(const void *in, size_t inlen, char *out,
                              size_t outcap, size_t *olen)
    __attribute__((alias("gj_mime_b64_word_encode")));
