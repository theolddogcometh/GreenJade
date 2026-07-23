/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch190: ASN.1 BER/DER OBJECT IDENTIFIER content
 * octets — arc (sub-identifier) encode / decode (X.690 §8.19).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party ASN.1 source was copied.
 *
 * Surface (unique symbols):
 *   int gj_oid_encode(const unsigned *arcs, size_t narc,
 *                     unsigned char *out, size_t cap, size_t *n);
 *     — Encode narc arcs into BER OID content octets (no Tag/Length).
 *       First two arcs a,b pack as one sub-identifier 40*a+b; remaining
 *       arcs each encode as base-128 (7-bit groups, MSB first, high bit
 *       set on all but the last octet of a group). *n = bytes written.
 *       Returns 0 on success, -1 on error.
 *   int gj_oid_decode(const unsigned char *in, size_t len,
 *                     unsigned *arcs, size_t acap, size_t *narc);
 *     — Inverse: content octets → arc list. *narc = arc count produced.
 *       Returns 0 on success, -1 on error.
 *   __gj_oid_encode / __gj_oid_decode  (aliases)
 *   __libcgj_batch190_marker = "libcgj-batch190"
 *
 * Rules (X.690):
 *   - narc >= 2; arcs[0] ∈ {0,1,2}; if arcs[0] < 2 then arcs[1] <= 39.
 *   - Base-128 is big-endian within each multi-octet sub-identifier
 *     (opposite of protobuf LEB128 / batch131).
 *   - Encode always emits the minimal form; decode rejects truncated
 *     streams, empty input, overflow past unsigned, and continuation
 *     past the 5-octet bound for a 32-bit arc.
 *   - No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch190_marker[] = "libcgj-batch190";

/*
 * Max base-128 octets for one 32-bit sub-identifier:
 *   ceil(32 / 7) = 5.
 */
#define B190_SUBID_MAX 5u

/* ---- base-128 helpers (ASN.1 OID, MSB first) --------------------------- */

/*
 * Return the number of octets needed to encode uVal as a BER OID
 * sub-identifier (minimal base-128). Always at least 1.
 */
static size_t
b190_subid_len(unsigned uVal)
{
	size_t cb;

	cb = 1u;
	uVal >>= 7;
	while (uVal != 0u) {
		cb++;
		uVal >>= 7;
	}
	return cb;
}

/*
 * Encode one sub-identifier into pOut[0..cbCap).
 * On success writes *pcb = bytes produced and returns 0.
 * Returns -1 if pOut is NULL (when needed) or cbCap is too small.
 */
static int
b190_enc_subid(unsigned uVal, unsigned char *pOut, size_t cbCap,
               size_t *pcb)
{
	unsigned char aTmp[B190_SUBID_MAX];
	size_t c;
	size_t i;
	unsigned u;

	/*
	 * Collect 7-bit groups low-to-high, then emit high-to-low so the
	 * most-significant group appears first (ASN.1, not LEB128).
	 */
	u = uVal;
	c = 0u;
	aTmp[c++] = (unsigned char)(u & 0x7fu);
	u >>= 7;
	while (u != 0u && c < B190_SUBID_MAX) {
		aTmp[c++] = (unsigned char)((u & 0x7fu) | 0x80u);
		u >>= 7;
	}
	if (u != 0u) {
		return -1;
	}

	if (pOut == NULL || cbCap < c) {
		return -1;
	}

	for (i = 0u; i < c; i++) {
		pOut[i] = aTmp[c - 1u - i];
	}
	*pcb = c;
	return 0;
}

/*
 * Decode one sub-identifier starting at pIn[0..cbLen).
 * *puVal = value, *pcb = bytes consumed. Returns 0 / -1.
 */
static int
b190_dec_subid(const unsigned char *pIn, size_t cbLen, unsigned *puVal,
               size_t *pcb)
{
	unsigned uVal;
	size_t i;
	unsigned char b;
	int fStarted;

	if (pIn == NULL || puVal == NULL || pcb == NULL || cbLen == 0u) {
		return -1;
	}

	uVal = 0u;
	fStarted = 0;
	for (i = 0u; i < cbLen && i < B190_SUBID_MAX; i++) {
		b = pIn[i];

		/*
		 * DER forbids non-minimal encodings: the first octet of a
		 * multi-octet sub-identifier must not be 0x80 (leading zero
		 * group). Accept single-octet 0x00.
		 */
		if (i == 0u && b == 0x80u) {
			return -1;
		}

		/*
		 * Shift in 7 data bits; reject if the high bits of unsigned
		 * would be displaced (value no longer fits in unsigned).
		 */
		if (fStarted != 0 && (uVal >> (sizeof(unsigned) * 8u - 7u)) != 0u) {
			return -1;
		}
		uVal = (uVal << 7) | (unsigned)(b & 0x7fu);
		fStarted = 1;

		if ((b & 0x80u) == 0u) {
			*puVal = uVal;
			*pcb = i + 1u;
			return 0;
		}
	}

	/* Truncated or still continuing after B190_SUBID_MAX octets. */
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oid_encode — arc list → BER OID content octets.
 *
 * arcs[0] must be 0, 1, or 2; if arcs[0] < 2 then arcs[1] must be <= 39.
 * At least two arcs are required. *pn receives the encoded length.
 */
int
gj_oid_encode(const unsigned *pArcs, size_t cArcs, unsigned char *pOut,
              size_t cbCap, size_t *pcbOut)
{
	unsigned uFirst;
	size_t cbOff;
	size_t cbPiece;
	size_t iArc;

	if (pArcs == NULL || pcbOut == NULL) {
		return -1;
	}
	if (cArcs < 2u) {
		return -1;
	}
	if (pArcs[0] > 2u) {
		return -1;
	}
	if (pArcs[0] < 2u && pArcs[1] > 39u) {
		return -1;
	}
	/*
	 * Combined first sub-identifier: 40*a + b. When a == 2, b may be
	 * large; guard against overflow of unsigned before packing
	 * (40*2 + b == 80 + b).
	 */
	if (pArcs[0] == 2u && pArcs[1] > (~0u - 80u)) {
		return -1;
	}

	uFirst = 40u * pArcs[0] + pArcs[1];

	/*
	 * Size pass when pOut is NULL: report required length in *pcbOut
	 * without writing. Still validate arcs.
	 */
	if (pOut == NULL) {
		size_t cbNeed;

		cbNeed = b190_subid_len(uFirst);
		for (iArc = 2u; iArc < cArcs; iArc++) {
			cbNeed += b190_subid_len(pArcs[iArc]);
		}
		*pcbOut = cbNeed;
		return 0;
	}

	cbOff = 0u;
	if (b190_enc_subid(uFirst, pOut + cbOff, cbCap - cbOff, &cbPiece) != 0) {
		return -1;
	}
	cbOff += cbPiece;

	for (iArc = 2u; iArc < cArcs; iArc++) {
		if (cbOff > cbCap) {
			return -1;
		}
		if (b190_enc_subid(pArcs[iArc], pOut + cbOff, cbCap - cbOff,
		                   &cbPiece) != 0) {
			return -1;
		}
		cbOff += cbPiece;
	}

	*pcbOut = cbOff;
	return 0;
}

int __gj_oid_encode(const unsigned *arcs, size_t narc, unsigned char *out,
                    size_t cap, size_t *n)
    __attribute__((alias("gj_oid_encode")));

/*
 * gj_oid_decode — BER OID content octets → arc list.
 *
 * Consumes the entire input. Produces at least two arcs from the first
 * sub-identifier (split via the 40*a+b convention). *pcArcs = count.
 */
int
gj_oid_decode(const unsigned char *pIn, size_t cbLen, unsigned *pArcs,
              size_t cAcap, size_t *pcArcs)
{
	unsigned uSub;
	size_t cbUsed;
	size_t cbOff;
	size_t cOut;

	if (pIn == NULL || pArcs == NULL || pcArcs == NULL) {
		return -1;
	}
	if (cbLen == 0u || cAcap < 2u) {
		return -1;
	}

	/* First sub-identifier → arcs[0], arcs[1]. */
	if (b190_dec_subid(pIn, cbLen, &uSub, &cbUsed) != 0) {
		return -1;
	}

	if (uSub < 40u) {
		pArcs[0] = 0u;
		pArcs[1] = uSub;
	} else if (uSub < 80u) {
		pArcs[0] = 1u;
		pArcs[1] = uSub - 40u;
	} else {
		pArcs[0] = 2u;
		pArcs[1] = uSub - 80u;
	}
	cOut = 2u;
	cbOff = cbUsed;

	while (cbOff < cbLen) {
		if (cOut >= cAcap) {
			return -1;
		}
		if (b190_dec_subid(pIn + cbOff, cbLen - cbOff, &uSub,
		                   &cbUsed) != 0) {
			return -1;
		}
		pArcs[cOut] = uSub;
		cOut++;
		cbOff += cbUsed;
	}

	*pcArcs = cOut;
	return 0;
}

int __gj_oid_decode(const unsigned char *in, size_t len, unsigned *arcs,
                    size_t acap, size_t *narc)
    __attribute__((alias("gj_oid_decode")));
