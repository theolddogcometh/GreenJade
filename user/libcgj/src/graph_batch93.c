/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch93: RFC 4648 §5 base64url encode/decode.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required).
 * Alphabet uses '-' / '_' instead of '+'/'; padding '=' may be present.
 * Standard base64 (+/) lives in graph_batch40.c — this TU is unique.
 *
 * Surface:
 *   base64url_encode / base64url_decode
 *   __base64url_encode / __base64url_decode  (aliases)
 *   __libcgj_batch93_marker = "libcgj-batch93"
 *
 * Encode API (matches base64/base32 graph shape):
 *   ssize_t base64url_encode(const unsigned char *in, size_t inlen,
 *                            char *out, size_t outcap);
 *   out == NULL  → returns bytes needed including trailing NUL
 *   success      → returns encoded length excluding NUL
 *   error        → -1 (EINVAL / ENOSPC)
 *
 * Decode: skips ASCII whitespace; accepts optional '=' padding and
 * unpadded tail (digit count % 4 == 2 or 3). Returns decoded byte count,
 * or -1 on error.
 *
 * Soft deepen: unpadded tail completion, post-pad trailing junk reject,
 * out==NULL length probe on encode, whitespace skip on decode.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch93_marker[] = "libcgj-batch93";

/* RFC 4648 base64url alphabet */
static const char s_b93_b64url[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static int
b93_is_ws(unsigned char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
	       c == '\v';
}

static int
b93_b64url_val(int ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return ch - 'A';
	}
	if (ch >= 'a' && ch <= 'z') {
		return ch - 'a' + 26;
	}
	if (ch >= '0' && ch <= '9') {
		return ch - '0' + 52;
	}
	if (ch == '-') {
		return 62;
	}
	if (ch == '_') {
		return 63;
	}
	return -1;
}

/*
 * Encode: always emits canonical 4-char groups with '=' padding
 * (same length shape as standard base64_encode).
 */
ssize_t
base64url_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                 size_t cbOut)
{
	size_t i = 0;
	size_t j = 0;
	size_t cbNeed = ((cbIn + 2u) / 3u) * 4u + 1u;

	if (pIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (szOut == NULL) {
		return (ssize_t)cbNeed;
	}
	if (cbOut < cbNeed) {
		errno = ENOSPC;
		return -1;
	}
	while (i < cbIn) {
		size_t nLeft = cbIn - i;
		unsigned u0 = pIn[i++];
		unsigned u1 = (nLeft > 1u) ? pIn[i++] : 0u;
		unsigned u2 = (nLeft > 2u) ? pIn[i++] : 0u;
		unsigned uTrip = (u0 << 16) | (u1 << 8) | u2;

		szOut[j++] = s_b93_b64url[(uTrip >> 18) & 63u];
		szOut[j++] = s_b93_b64url[(uTrip >> 12) & 63u];
		if (nLeft > 1u) {
			szOut[j++] = s_b93_b64url[(uTrip >> 6) & 63u];
		} else {
			szOut[j++] = '=';
		}
		if (nLeft > 2u) {
			szOut[j++] = s_b93_b64url[uTrip & 63u];
		} else {
			szOut[j++] = '=';
		}
	}
	szOut[j] = '\0';
	return (ssize_t)j;
}

static int
b93_emit(unsigned uTrip, int nOut, unsigned char *pOut, size_t cbOut,
         size_t *pJ)
{
	size_t j = *pJ;

	if (pOut == NULL) {
		*pJ = j + (size_t)nOut;
		return 0;
	}
	if (j + (size_t)nOut > cbOut) {
		errno = ENOSPC;
		return -1;
	}
	pOut[j++] = (unsigned char)(uTrip >> 16);
	if (nOut > 1) {
		pOut[j++] = (unsigned char)(uTrip >> 8);
	}
	if (nOut > 2) {
		pOut[j++] = (unsigned char)uTrip;
	}
	*pJ = j;
	return 0;
}

/*
 * Decode one logical quartet of four input characters (alphabet or '=').
 * Matches graph_batch40 base64_decode quartet rules with base64url values.
 */
static int
b93_decode_quartet(const char aC[4], unsigned char *pOut, size_t cbOut,
                   size_t *pJ)
{
	int v0, v1, v2, v3;
	unsigned uTrip;
	int nOut;
	char c2 = aC[2];
	char c3 = aC[3];

	v0 = b93_b64url_val((unsigned char)aC[0]);
	v1 = b93_b64url_val((unsigned char)aC[1]);
	if (v0 < 0 || v1 < 0) {
		errno = EINVAL;
		return -1;
	}
	if (c2 == '=') {
		v2 = 0;
		nOut = 1;
		if (c3 != '=') {
			errno = EINVAL;
			return -1;
		}
		v3 = 0;
	} else {
		v2 = b93_b64url_val((unsigned char)c2);
		if (v2 < 0) {
			errno = EINVAL;
			return -1;
		}
		if (c3 == '=') {
			v3 = 0;
			nOut = 2;
		} else {
			v3 = b93_b64url_val((unsigned char)c3);
			if (v3 < 0) {
				errno = EINVAL;
				return -1;
			}
			nOut = 3;
		}
	}
	uTrip = ((unsigned)v0 << 18) | ((unsigned)v1 << 12) |
	        ((unsigned)v2 << 6) | (unsigned)v3;
	return b93_emit(uTrip, nOut, pOut, cbOut, pJ);
}

ssize_t
base64url_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
                 size_t cbOut)
{
	size_t i = 0;
	size_t j = 0;
	char aQ[4];
	unsigned nQ = 0;
	int bPadSeen = 0;

	if (szIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}

	while (i < cbIn) {
		unsigned char c = (unsigned char)szIn[i++];

		if (b93_is_ws(c)) {
			continue;
		}
		if (bPadSeen) {
			/* After a completed padded quartet, only WS is allowed. */
			errno = EINVAL;
			return -1;
		}
		if (c == (unsigned char)'=') {
			if (nQ < 2u) {
				errno = EINVAL;
				return -1;
			}
			aQ[nQ++] = '=';
			if (nQ == 4u) {
				if (b93_decode_quartet(aQ, pOut, cbOut, &j) != 0) {
					return -1;
				}
				nQ = 0;
				bPadSeen = 1;
			}
			continue;
		}
		if (b93_b64url_val((int)c) < 0) {
			errno = EINVAL;
			return -1;
		}
		/* Alphabet digit cannot follow pad within a quartet. */
		if (nQ > 0u && aQ[nQ - 1u] == '=') {
			errno = EINVAL;
			return -1;
		}
		aQ[nQ++] = (char)c;
		if (nQ == 4u) {
			if (b93_decode_quartet(aQ, pOut, cbOut, &j) != 0) {
				return -1;
			}
			nQ = 0;
		}
	}

	/* Unpadded tail: 0, 2, or 3 alphabet digits (1 is invalid). */
	if (nQ == 0u) {
		return (ssize_t)j;
	}
	if (nQ == 1u) {
		errno = EINVAL;
		return -1;
	}
	/* Incomplete padded quartet (e.g. only one '=' of "==") is invalid. */
	{
		unsigned k;

		for (k = 0; k < nQ; k++) {
			if (aQ[k] == '=') {
				errno = EINVAL;
				return -1;
			}
		}
	}
	if (nQ == 2u) {
		aQ[2] = '=';
		aQ[3] = '=';
	} else {
		/* nQ == 3 */
		aQ[3] = '=';
	}
	if (b93_decode_quartet(aQ, pOut, cbOut, &j) != 0) {
		return -1;
	}
	return (ssize_t)j;
}

ssize_t __base64url_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base64url_encode")));
ssize_t __base64url_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base64url_decode")));
