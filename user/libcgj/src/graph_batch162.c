/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch162: JWT compact structure split + base64url
 * segment decode (no crypto / no signature verify).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party JWT / JOSE source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base64url_encode / base64url_decode  → graph_batch93.c (ssize_t API)
 *   base64_encode / base64_decode        → graph_batch40.c
 *   b64_ntop / b64_pton                  → graph_batch22.c
 *
 * This TU adds only unique symbols (structure parse only; no HMAC/RSA):
 *   int gj_jwt_split(const char *jwt,
 *                    const char **h, size_t *hl,
 *                    const char **p, size_t *pl,
 *                    const char **s, size_t *sl);
 *     — Split JWS/JWT compact form "header.payload.signature" into three
 *       segments (pointers into jwt, lengths). Exactly two '.' delimiters.
 *       Empty segments allowed (structure only). Returns 0 / -1.
 *   int gj_jwt_b64url_decode_seg(const char *seg, size_t seglen,
 *                                unsigned char *out, size_t cap,
 *                                size_t *olen);
 *     — RFC 4648 §5 base64url decode of one segment. Accepts optional '='
 *       padding and unpadded tails (seglen % 4 != 1). out may be NULL to
 *       size-query via *olen. Returns 0 / -1; no errno.
 *   __gj_jwt_split / __gj_jwt_b64url_decode_seg  (aliases)
 *   __libcgj_batch162_marker = "libcgj-batch162"
 *
 * Notes:
 *   - Does NOT verify alg/signature, does NOT parse JSON claims.
 *   - Alphabet: A-Za-z0-9-_  (no '+' '/'). Whitespace rejected.
 */

#include <stddef.h>

const char __libcgj_batch162_marker[] = "libcgj-batch162";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map one base64url alphabet character to 0..63, or -1 if invalid.
 * Does not accept standard base64 '+' or '/'.
 */
static int
b162_b64url_val(unsigned char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (int)(ch - 'A');
	}
	if (ch >= 'a' && ch <= 'z') {
		return (int)(ch - 'a' + 26);
	}
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0' + 52);
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
 * Emit nOut (1..3) decoded bytes from a 24-bit triple into pOut if non-NULL.
 * Advances *pcbOut. Returns 0, or -1 if capacity is insufficient.
 */
static int
b162_emit(unsigned uTrip, int nOut, unsigned char *pOut, size_t cbCap,
          size_t *pcbOut)
{
	size_t cbCur = *pcbOut;

	if (pOut == NULL) {
		*pcbOut = cbCur + (size_t)nOut;
		return 0;
	}
	if (cbCur + (size_t)nOut > cbCap) {
		return -1;
	}
	pOut[cbCur] = (unsigned char)((uTrip >> 16) & 0xffu);
	if (nOut > 1) {
		pOut[cbCur + 1u] = (unsigned char)((uTrip >> 8) & 0xffu);
	}
	if (nOut > 2) {
		pOut[cbCur + 2u] = (unsigned char)(uTrip & 0xffu);
	}
	*pcbOut = cbCur + (size_t)nOut;
	return 0;
}

/*
 * Decode one logical quartet (4 chars, possibly with '=' pad) into 1..3
 * output bytes. aC[0..1] must be alphabet; aC[2..3] alphabet or '='.
 */
static int
b162_decode_quartet(const char aC[4], unsigned char *pOut, size_t cbCap,
                    size_t *pcbOut)
{
	int nV0;
	int nV1;
	int nV2;
	int nV3;
	int nOut;
	unsigned uTrip;
	unsigned char ch2;
	unsigned char ch3;

	nV0 = b162_b64url_val((unsigned char)aC[0]);
	nV1 = b162_b64url_val((unsigned char)aC[1]);
	if (nV0 < 0 || nV1 < 0) {
		return -1;
	}

	ch2 = (unsigned char)aC[2];
	ch3 = (unsigned char)aC[3];

	if (ch2 == (unsigned char)'=') {
		/* "xx==" → 1 byte; third pad requires fourth pad */
		if (ch3 != (unsigned char)'=') {
			return -1;
		}
		nV2 = 0;
		nV3 = 0;
		nOut = 1;
	} else {
		nV2 = b162_b64url_val(ch2);
		if (nV2 < 0) {
			return -1;
		}
		if (ch3 == (unsigned char)'=') {
			/* "xxx=" → 2 bytes */
			nV3 = 0;
			nOut = 2;
		} else {
			nV3 = b162_b64url_val(ch3);
			if (nV3 < 0) {
				return -1;
			}
			nOut = 3;
		}
	}

	uTrip = ((unsigned)nV0 << 18) | ((unsigned)nV1 << 12) |
	        ((unsigned)nV2 << 6) | (unsigned)nV3;
	return b162_emit(uTrip, nOut, pOut, cbCap, pcbOut);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jwt_split — split compact JWT/JWS "header.payload.signature".
 *
 * Requires exactly two '.' characters (no more, no less). Segment
 * pointers reference into szJwt (not copied). Lengths exclude the dots.
 * Empty segments are accepted (structure-only; no JSON/crypto checks).
 *
 * Returns 0 on success, -1 on NULL args or malformed shape.
 */
int
gj_jwt_split(const char *szJwt, const char **ppH, size_t *pcbH,
             const char **ppP, size_t *pcbP, const char **ppS, size_t *pcbS)
{
	const char *pDot1;
	const char *pDot2;
	const char *pScan;
	size_t cbH;
	size_t cbP;
	size_t cbS;

	if (szJwt == NULL || ppH == NULL || pcbH == NULL || ppP == NULL ||
	    pcbP == NULL || ppS == NULL || pcbS == NULL) {
		return -1;
	}

	/* first separator */
	pDot1 = szJwt;
	while (*pDot1 != '\0' && *pDot1 != '.') {
		pDot1++;
	}
	if (*pDot1 != '.') {
		return -1;
	}

	/* second separator */
	pDot2 = pDot1 + 1;
	while (*pDot2 != '\0' && *pDot2 != '.') {
		pDot2++;
	}
	if (*pDot2 != '.') {
		return -1;
	}

	/* no further '.' in the signature segment */
	pScan = pDot2 + 1;
	while (*pScan != '\0') {
		if (*pScan == '.') {
			return -1;
		}
		pScan++;
	}

	cbH = (size_t)(pDot1 - szJwt);
	cbP = (size_t)(pDot2 - (pDot1 + 1));
	cbS = (size_t)(pScan - (pDot2 + 1));

	*ppH = szJwt;
	*pcbH = cbH;
	*ppP = pDot1 + 1;
	*pcbP = cbP;
	*ppS = pDot2 + 1;
	*pcbS = cbS;
	return 0;
}

int __gj_jwt_split(const char *szJwt, const char **ppH, size_t *pcbH,
                   const char **ppP, size_t *pcbP, const char **ppS,
                   size_t *pcbS)
    __attribute__((alias("gj_jwt_split")));

/*
 * gj_jwt_b64url_decode_seg — decode one base64url JWT segment.
 *
 * Rules (RFC 4648 §5 + JOSE unpadded practice):
 *   - Alphabet A-Za-z0-9-_ only (plus optional trailing '=' pad)
 *   - No whitespace
 *   - seglen % 4 == 1 is invalid
 *   - Unpadded tail of 2 or 3 alphabet digits is accepted
 *   - pOut == NULL → size query only (*pcbOlen set, return 0)
 *   - pOut non-NULL and cbCap too small → -1
 *
 * On success *pcbOlen is the decoded byte count and return is 0.
 * On error return is -1 (*pcbOlen may be left unspecified).
 */
int
gj_jwt_b64url_decode_seg(const char *pSeg, size_t cbSeg, unsigned char *pOut,
                         size_t cbCap, size_t *pcbOlen)
{
	size_t i;
	size_t cbOut;
	char aQ[4];
	unsigned nQ;
	int fPadSeen;

	if (pcbOlen == NULL) {
		return -1;
	}
	if (pSeg == NULL && cbSeg != 0u) {
		return -1;
	}

	cbOut = 0u;
	nQ = 0u;
	fPadSeen = 0;
	i = 0u;

	while (i < cbSeg) {
		unsigned char ch = (unsigned char)pSeg[i++];

		if (fPadSeen) {
			/* after a completed padded quartet, no more input */
			return -1;
		}

		if (ch == (unsigned char)'=') {
			if (nQ < 2u) {
				return -1;
			}
			aQ[nQ++] = '=';
			if (nQ == 4u) {
				if (b162_decode_quartet(aQ, pOut, cbCap,
				                        &cbOut) != 0) {
					return -1;
				}
				nQ = 0u;
				fPadSeen = 1;
			}
			continue;
		}

		if (b162_b64url_val(ch) < 0) {
			return -1;
		}
		/* alphabet digit cannot follow pad within a quartet */
		if (nQ > 0u && aQ[nQ - 1u] == '=') {
			return -1;
		}
		aQ[nQ++] = (char)ch;
		if (nQ == 4u) {
			if (b162_decode_quartet(aQ, pOut, cbCap, &cbOut) != 0) {
				return -1;
			}
			nQ = 0u;
		}
	}

	/* Unpadded tail: 0, 2, or 3 alphabet digits (1 is invalid). */
	if (nQ == 0u) {
		*pcbOlen = cbOut;
		return 0;
	}
	if (nQ == 1u) {
		return -1;
	}
	if (nQ == 2u) {
		/* "xx" → one byte; pad virtually as "xx==" */
		aQ[2] = '=';
		aQ[3] = '=';
	} else {
		/* nQ == 3: "xxx" → two bytes; pad virtually as "xxx=" */
		aQ[3] = '=';
	}
	if (b162_decode_quartet(aQ, pOut, cbCap, &cbOut) != 0) {
		return -1;
	}
	*pcbOlen = cbOut;
	return 0;
}

int __gj_jwt_b64url_decode_seg(const char *pSeg, size_t cbSeg,
                               unsigned char *pOut, size_t cbCap,
                               size_t *pcbOlen)
    __attribute__((alias("gj_jwt_b64url_decode_seg")));
