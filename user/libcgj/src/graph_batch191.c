/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch191: SubjectPublicKeyInfo outer TLV peel —
 * locate the subjectPublicKey BIT STRING payload inside a DER SPKI.
 * Clean-room freestanding pure C from the public ASN.1 layout
 * (RFC 5280 §4.1 / X.509 SubjectPublicKeyInfo):
 *   SubjectPublicKeyInfo ::= SEQUENCE {
 *       algorithm        AlgorithmIdentifier,
 *       subjectPublicKey BIT STRING }
 * Builds under userspace -msse2. No third-party ASN.1/DER source was
 * copied. No malloc, no errno (freestanding-safe).
 *
 * Surface (unique symbols):
 *   int gj_spki_find_bitstring(const unsigned char *der, size_t len,
 *                              const unsigned char **bits, size_t *blen,
 *                              int *unused_bits);
 *     Peel outer SEQUENCE, skip AlgorithmIdentifier (first TLV), then
 *     parse the BIT STRING. On success:
 *       *unused_bits = first content octet (0..7)
 *       *bits        = pointer to remaining content (key bits)
 *       *blen        = content_length - 1
 *     Returns 0 on success, -1 on malformed/NULL/truncated input.
 *   __gj_spki_find_bitstring  (alias)
 *   __libcgj_batch191_marker = "libcgj-batch191"
 *
 * Related (not redefined here):
 *   gj_pem_find / gj_pem_decode  → graph_batch115.c
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch191_marker[] = "libcgj-batch191";

/* Universal class tags of interest (single-byte, low-tag-number form). */
#define B191_TAG_BIT_STRING 0x03u
#define B191_TAG_SEQUENCE   0x30u

/* ---- DER TLV header ---------------------------------------------------- */

/*
 * Parse one DER TLV header at pDer[0..cbLen).
 *
 * On success writes:
 *   *puTag   — tag byte (single-byte tags only; multi-byte tags rejected)
 *   *pcbHdr  — octets consumed by tag + length fields
 *   *pcbVal  — definite content length in octets
 * and returns 0. Rejects indefinite length, truncated streams, non-minimal
 * long-form lengths, and lengths that would exceed the remaining buffer.
 */
static int
b191_tlv_hdr(const unsigned char *pDer, size_t cbLen, unsigned *puTag,
             size_t *pcbHdr, size_t *pcbVal)
{
	unsigned uTag;
	unsigned uLen0;
	unsigned uNb;
	unsigned uI;
	size_t cbVal;
	size_t cbHdr;

	if (pDer == NULL || puTag == NULL || pcbHdr == NULL ||
	    pcbVal == NULL || cbLen < 2u) {
		return -1;
	}

	uTag = (unsigned)pDer[0];
	/* High-tag-number form (tag bits 0..4 all set) not accepted here. */
	if ((uTag & 0x1fu) == 0x1fu) {
		return -1;
	}

	uLen0 = (unsigned)pDer[1];
	if ((uLen0 & 0x80u) == 0u) {
		/* Short form: length is uLen0 (0..127). */
		cbVal = (size_t)uLen0;
		cbHdr = 2u;
	} else {
		/*
		 * Long form: low 7 bits = number of subsequent length octets.
		 * DER forbids indefinite (0x80) and overlong / leading-zero
		 * encodings.
		 */
		uNb = uLen0 & 0x7fu;
		if (uNb == 0u || uNb > 4u) {
			/* Indefinite or absurdly large length field. */
			return -1;
		}
		if (cbLen < 2u + (size_t)uNb) {
			return -1;
		}
		/* Leading zero is non-minimal (and short form is required
		 * for values that fit in one length octet without 0x80). */
		if (pDer[2] == 0u) {
			return -1;
		}
		cbVal = 0u;
		for (uI = 0u; uI < uNb; uI++) {
			cbVal = (cbVal << 8) | (size_t)pDer[2u + uI];
		}
		/* Values that fit in short form must use short form. */
		if (uNb == 1u && cbVal < 0x80u) {
			return -1;
		}
		cbHdr = 2u + (size_t)uNb;
	}

	/* Content must fit in the remaining buffer after the header. */
	if (cbHdr > cbLen || cbVal > cbLen - cbHdr) {
		return -1;
	}

	*puTag = uTag;
	*pcbHdr = cbHdr;
	*pcbVal = cbVal;
	return 0;
}

/* ---- gj_spki_find_bitstring -------------------------------------------- */

/*
 * Peel a DER-encoded SubjectPublicKeyInfo and return the BIT STRING
 * payload (subjectPublicKey bits) plus the leading unused-bits count.
 *
 * Walk:
 *   1) outer SEQUENCE (0x30)
 *   2) skip first child TLV (AlgorithmIdentifier)
 *   3) require second child TLV is BIT STRING (0x03)
 *   4) BIT STRING content = unused_bits (1 octet) || bit_payload
 *
 * All of bits/blen/unused_bits must be non-NULL. On failure no out
 * parameter is written.
 */
int
gj_spki_find_bitstring(const unsigned char *pDer, size_t cbLen,
                       const unsigned char **ppBits, size_t *pcbBits,
                       int *pnUnusedBits)
{
	unsigned uTag;
	size_t cbHdr;
	size_t cbVal;
	size_t cbOuter;
	const unsigned char *pCur;
	size_t cbLeft;
	const unsigned char *pAlg;
	size_t cbAlgHdr;
	size_t cbAlgVal;
	const unsigned char *pBs;
	size_t cbBsHdr;
	size_t cbBsVal;
	unsigned uUnused;

	if (pDer == NULL || ppBits == NULL || pcbBits == NULL ||
	    pnUnusedBits == NULL) {
		return -1;
	}

	/* Outer SubjectPublicKeyInfo SEQUENCE. */
	if (b191_tlv_hdr(pDer, cbLen, &uTag, &cbHdr, &cbVal) != 0) {
		return -1;
	}
	if (uTag != B191_TAG_SEQUENCE) {
		return -1;
	}

	pCur = pDer + cbHdr;
	cbLeft = cbVal;
	cbOuter = cbVal;
	(void)cbOuter;

	/* First child: AlgorithmIdentifier (typically SEQUENCE); skip whole TLV. */
	if (b191_tlv_hdr(pCur, cbLeft, &uTag, &cbAlgHdr, &cbAlgVal) != 0) {
		return -1;
	}
	/* Full TLV size = header + value; must fit. */
	if (cbAlgHdr + cbAlgVal > cbLeft) {
		return -1;
	}
	pAlg = pCur;
	(void)pAlg;
	pCur += cbAlgHdr + cbAlgVal;
	cbLeft -= cbAlgHdr + cbAlgVal;

	/* Second child: subjectPublicKey BIT STRING. */
	if (b191_tlv_hdr(pCur, cbLeft, &uTag, &cbBsHdr, &cbBsVal) != 0) {
		return -1;
	}
	if (uTag != B191_TAG_BIT_STRING) {
		return -1;
	}
	if (cbBsHdr + cbBsVal > cbLeft) {
		return -1;
	}
	/* BIT STRING content must include the unused-bits octet. */
	if (cbBsVal < 1u) {
		return -1;
	}

	pBs = pCur + cbBsHdr;
	uUnused = (unsigned)pBs[0];
	/* Unused bits is 0..7; empty bit string requires unused == 0. */
	if (uUnused > 7u) {
		return -1;
	}
	if (cbBsVal == 1u && uUnused != 0u) {
		return -1;
	}

	*ppBits = pBs + 1u;
	*pcbBits = cbBsVal - 1u;
	*pnUnusedBits = (int)uUnused;
	return 0;
}

int __gj_spki_find_bitstring(const unsigned char *der, size_t len,
                             const unsigned char **bits, size_t *blen,
                             int *unused_bits)
    __attribute__((alias("gj_spki_find_bitstring")));
