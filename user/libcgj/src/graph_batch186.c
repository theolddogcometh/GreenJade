/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch186: base64url encode/decode with optional
 * '=' padding control (RFC 4648 §5 alphabet).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2; this TU does not require SSE). No third-party
 * base64url source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   base64url_encode / base64url_decode  → graph_batch93.c (ssize_t, errno)
 *   base64_encode / base64_decode        → graph_batch40.c
 *   gj_jwt_b64url_decode_seg             → graph_batch162.c (JWT segment)
 *   b64_ntop / b64_pton                  → graph_batch22.c
 *
 * This TU adds only unique symbols:
 *   int gj_b64url_encode_pad(const unsigned char *in, size_t inlen,
 *                            char *out, size_t outcap, size_t *olen,
 *                            int pad);
 *     — RFC 4648 §5 encode (A-Za-z0-9-_). pad != 0 emits canonical '='
 *       padding to a multiple of 4; pad == 0 emits unpadded output
 *       (JOSE/JWT practice). out may be NULL to size-query via *olen.
 *       *olen = encoded char count (no trailing NUL). Returns 0 / -1.
 *   int gj_b64url_decode_pad(const char *in, size_t inlen,
 *                            unsigned char *out, size_t outcap,
 *                            size_t *olen);
 *     — inverse. Accepts optional trailing '=' padding and unpadded
 *       tails of 2 or 3 alphabet digits (inlen % 4 == 1 invalid).
 *       Whitespace rejected. out may be NULL to size-query via *olen.
 *       Returns 0 / -1; no errno.
 *   __gj_b64url_encode_pad / __gj_b64url_decode_pad  (aliases)
 *   __libcgj_batch186_marker = "libcgj-batch186"
 *
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch186_marker[] = "libcgj-batch186";

/* RFC 4648 §5 base64url alphabet */
static const char s_b186_alpha[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map one base64url alphabet character to 0..63, or -1 if invalid.
 * Does not accept standard base64 '+' or '/'.
 */
static int
b186_val(unsigned char ch)
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
b186_emit(unsigned uTrip, int nOut, unsigned char *pOut, size_t cbCap,
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
b186_decode_quartet(const char aC[4], unsigned char *pOut, size_t cbCap,
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

	nV0 = b186_val((unsigned char)aC[0]);
	nV1 = b186_val((unsigned char)aC[1]);
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
		nV2 = b186_val(ch2);
		if (nV2 < 0) {
			return -1;
		}
		if (ch3 == (unsigned char)'=') {
			/* "xxx=" → 2 bytes */
			nV3 = 0;
			nOut = 2;
		} else {
			nV3 = b186_val(ch3);
			if (nV3 < 0) {
				return -1;
			}
			nOut = 3;
		}
	}

	uTrip = ((unsigned)nV0 << 18) | ((unsigned)nV1 << 12) |
	        ((unsigned)nV2 << 6) | (unsigned)nV3;
	return b186_emit(uTrip, nOut, pOut, cbCap, pcbOut);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64url_encode_pad — base64url encode with optional '=' padding.
 *
 * pad != 0: emit canonical 4-char groups with '=' (RFC 4648 length shape).
 * pad == 0: omit trailing '=' (JOSE unpadded). Full 3-byte groups always
 * emit 4 alphabet digits either way.
 *
 * pIn may be NULL only when cbIn == 0. pOut may be NULL to size-query
 * via *pcbOlen. On success *pcbOlen is the encoded character count
 * (no trailing NUL is written). Returns 0 on success, -1 on error.
 */
int
gj_b64url_encode_pad(const unsigned char *pIn, size_t cbIn, char *pOut,
                     size_t cbCap, size_t *pcbOlen, int nPad)
{
	size_t i;
	size_t o;
	size_t cbNeed;
	size_t nFull;
	size_t nRem;

	if (pcbOlen == NULL) {
		return -1;
	}
	if (pIn == NULL && cbIn != 0u) {
		return -1;
	}

	nFull = cbIn / 3u;
	nRem = cbIn % 3u;

	if (nPad != 0) {
		/* padded: always 4 chars per residual group */
		cbNeed = nFull * 4u + (nRem != 0u ? 4u : 0u);
	} else {
		/* unpadded: 2 chars for 1 leftover byte, 3 for 2 */
		cbNeed = nFull * 4u;
		if (nRem == 1u) {
			cbNeed += 2u;
		} else if (nRem == 2u) {
			cbNeed += 3u;
		}
	}

	if (pOut == NULL) {
		*pcbOlen = cbNeed;
		return 0;
	}
	if (cbNeed > cbCap) {
		return -1;
	}

	o = 0u;
	i = 0u;
	while (i + 3u <= cbIn) {
		unsigned uTrip = ((unsigned)pIn[i] << 16) |
		                 ((unsigned)pIn[i + 1u] << 8) |
		                 (unsigned)pIn[i + 2u];

		pOut[o++] = s_b186_alpha[(uTrip >> 18) & 63u];
		pOut[o++] = s_b186_alpha[(uTrip >> 12) & 63u];
		pOut[o++] = s_b186_alpha[(uTrip >> 6) & 63u];
		pOut[o++] = s_b186_alpha[uTrip & 63u];
		i += 3u;
	}

	if (nRem == 1u) {
		unsigned uTrip = (unsigned)pIn[i] << 16;

		pOut[o++] = s_b186_alpha[(uTrip >> 18) & 63u];
		pOut[o++] = s_b186_alpha[(uTrip >> 12) & 63u];
		if (nPad != 0) {
			pOut[o++] = '=';
			pOut[o++] = '=';
		}
	} else if (nRem == 2u) {
		unsigned uTrip = ((unsigned)pIn[i] << 16) |
		                 ((unsigned)pIn[i + 1u] << 8);

		pOut[o++] = s_b186_alpha[(uTrip >> 18) & 63u];
		pOut[o++] = s_b186_alpha[(uTrip >> 12) & 63u];
		pOut[o++] = s_b186_alpha[(uTrip >> 6) & 63u];
		if (nPad != 0) {
			pOut[o++] = '=';
		}
	}

	*pcbOlen = o;
	return 0;
}

int __gj_b64url_encode_pad(const unsigned char *pIn, size_t cbIn, char *pOut,
                           size_t cbCap, size_t *pcbOlen, int nPad)
    __attribute__((alias("gj_b64url_encode_pad")));

/*
 * gj_b64url_decode_pad — base64url decode accepting optional '=' padding.
 *
 * Rules (RFC 4648 §5 + JOSE unpadded practice):
 *   - Alphabet A-Za-z0-9-_ only (plus optional trailing '=')
 *   - No whitespace
 *   - input length % 4 == 1 is invalid (after padding synthesis)
 *   - Unpadded tail of 2 or 3 alphabet digits is accepted
 *   - pOut == NULL → size query only (*pcbOlen set, return 0)
 *   - pOut non-NULL and cbCap too small → -1
 *
 * On success *pcbOlen is the decoded byte count and return is 0.
 * On error return is -1 (*pcbOlen may be left unspecified).
 */
int
gj_b64url_decode_pad(const char *pIn, size_t cbIn, unsigned char *pOut,
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
	if (pIn == NULL && cbIn != 0u) {
		return -1;
	}

	cbOut = 0u;
	nQ = 0u;
	fPadSeen = 0;
	i = 0u;

	while (i < cbIn) {
		unsigned char ch = (unsigned char)pIn[i++];

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
				if (b186_decode_quartet(aQ, pOut, cbCap,
				                        &cbOut) != 0) {
					return -1;
				}
				nQ = 0u;
				fPadSeen = 1;
			}
			continue;
		}

		if (b186_val(ch) < 0) {
			return -1;
		}
		/* alphabet digit cannot follow pad within a quartet */
		if (nQ > 0u && aQ[nQ - 1u] == '=') {
			return -1;
		}
		aQ[nQ++] = (char)ch;
		if (nQ == 4u) {
			if (b186_decode_quartet(aQ, pOut, cbCap, &cbOut) != 0) {
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
	/* Incomplete padded quartet (e.g. only one '=' of "==") is invalid. */
	{
		unsigned k;

		for (k = 0u; k < nQ; k++) {
			if (aQ[k] == '=') {
				return -1;
			}
		}
	}
	if (nQ == 2u) {
		/* "xx" → one byte; pad virtually as "xx==" */
		aQ[2] = '=';
		aQ[3] = '=';
	} else {
		/* nQ == 3: "xxx" → two bytes; pad virtually as "xxx=" */
		aQ[3] = '=';
	}
	if (b186_decode_quartet(aQ, pOut, cbCap, &cbOut) != 0) {
		return -1;
	}
	*pcbOlen = cbOut;
	return 0;
}

int __gj_b64url_decode_pad(const char *pIn, size_t cbIn, unsigned char *pOut,
                           size_t cbCap, size_t *pcbOlen)
    __attribute__((alias("gj_b64url_decode_pad")));
