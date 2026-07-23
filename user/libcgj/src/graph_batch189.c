/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch189: ASN.1 DER TLV header parse (X.690).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party ASN.1 source was copied.
 *
 * Surface (unique symbols):
 *   int gj_asn1_tlv(const unsigned char *in, size_t len,
 *                   unsigned *tag, size_t *hdr_len, size_t *val_len);
 *     Parse one DER TLV from in[0..len). On success (0):
 *       *tag     = identifier: low-tag-number form → the single identifier
 *                  octet (class|P/C|number, e.g. 0x02 INTEGER, 0x30 SEQUENCE);
 *                  high-tag-number form → decoded multi-byte tag number
 *                  (>= 31). Class/constructed for high form remain as
 *                  (in[0] & 0xE0).
 *       *hdr_len = bytes consumed by identifier + length fields
 *       *val_len = content length (definite form only)
 *     Requires the full content to be present (hdr_len + val_len <= len).
 *     Returns -1 on NULL args, truncated header/value, BER indefinite
 *     length, non-minimal DER length/tag encoding, or overflow.
 *   __gj_asn1_tlv  (alias)
 *   __libcgj_batch189_marker = "libcgj-batch189"
 *
 * DER TLV layout (X.690):
 *   Identifier:  class(2) | constructed(1) | tag_number(5)
 *                tag_number 0..30 in low 5 bits; 0x1F → high-tag base-128
 *   Length:      short form 0x00..0x7F; long form 0x81..0xFE + BE octets
 *                (0x80 indefinite forbidden in DER; long form minimal)
 *   Contents:    val_len octets
 *
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch189_marker[] = "libcgj-batch189";

/* Max subsequent octets for a high-tag number (fits comfortably in unsigned). */
#define B189_TAG_OCTETS_MAX  4u
/* Max length-of-length octets (size_t is at most 8 bytes on LP64). */
#define B189_LEN_OCTETS_MAX  8u

/* ---- gj_asn1_tlv ------------------------------------------------------- */

/*
 * Parse a single DER-encoded TLV header (and require content present).
 *
 * Identifier (X.690 §8.1.2):
 *   Low-tag-number:  (in[0] & 0x1F) != 0x1F → *tag = in[0]
 *   High-tag-number: (in[0] & 0x1F) == 0x1F → subsequent octets base-128
 *     big-endian tag number; DER forbids leading 0x80 (non-minimal).
 *
 * Length (X.690 §8.1.3, DER definite + minimal):
 *   Short: bit7 clear → length = octet
 *   Long:  bit7 set, n = bits6..0 (>0); next n octets BE length;
 *          n==0 is indefinite (BER-only) → reject;
 *          leading 0x00 forbidden; short form required when L <= 127.
 */
int
gj_asn1_tlv(const unsigned char *in, size_t len, unsigned *tag,
            size_t *hdr_len, size_t *val_len)
{
	size_t i;
	unsigned uTag;
	size_t cbVal;
	size_t cbHdr;
	unsigned char bLen;
	unsigned nLenOct;

	if (in == NULL || tag == NULL || hdr_len == NULL || val_len == NULL) {
		return -1;
	}
	if (len == 0u) {
		return -1;
	}

	/* ---- identifier ---- */
	i = 0u;
	if ((in[0] & 0x1fu) != 0x1fu) {
		/* Low-tag-number form: single octet is the full identifier. */
		uTag = (unsigned)in[0];
		i = 1u;
	} else {
		/*
		 * High-tag-number form: accumulate base-128 tag number from
		 * subsequent octets. Need at least one subsequent octet.
		 */
		unsigned uNum;
		unsigned nOct;
		int bCont;

		i = 1u;
		if (i >= len) {
			return -1;
		}
		/*
		 * DER: first subsequent octet must not be 0x80 (would encode
		 * a leading zero 7-bit group — non-minimal).
		 */
		if (in[i] == 0x80u) {
			return -1;
		}

		uNum = 0u;
		nOct = 0u;
		bCont = 1;
		while (bCont) {
			unsigned char b;
			unsigned dig;

			if (i >= len || nOct >= B189_TAG_OCTETS_MAX) {
				return -1;
			}
			b = in[i];
			i++;
			nOct++;
			dig = (unsigned)(b & 0x7fu);
			/*
			 * Reject shift that would overflow unsigned (assume
			 * at least 32-bit unsigned; keep room for 7 bits).
			 */
			if (uNum > (unsigned)(~0u >> 7)) {
				return -1;
			}
			uNum = (uNum << 7) | dig;
			bCont = ((b & 0x80u) != 0u) ? 1 : 0;
		}
		/*
		 * High-tag form is only legal for tag numbers >= 31; a
		 * smaller number would be non-minimal DER.
		 */
		if (uNum < 31u) {
			return -1;
		}
		uTag = uNum;
	}

	/* ---- length ---- */
	if (i >= len) {
		return -1;
	}
	bLen = in[i];
	i++;

	if ((bLen & 0x80u) == 0u) {
		/* Short form: length is the low 7 bits. */
		cbVal = (size_t)bLen;
	} else {
		nLenOct = (unsigned)(bLen & 0x7fu);
		/* nLenOct == 0 → indefinite length (BER only). */
		if (nLenOct == 0u) {
			return -1;
		}
		if (nLenOct > B189_LEN_OCTETS_MAX) {
			return -1;
		}
		if (i + (size_t)nLenOct > len) {
			return -1;
		}
		/*
		 * DER: long-form length must be minimal — no leading 0x00,
		 * and must not encode a value that fits in the short form
		 * (i.e. L <= 127 requires short form).
		 */
		if (in[i] == 0x00u) {
			return -1;
		}

		{
			size_t uAcc;
			unsigned k;

			uAcc = 0u;
			for (k = 0u; k < nLenOct; k++) {
				/* Overflow if shifting would lose bits. */
				if (uAcc > ((size_t)~(size_t)0 >> 8)) {
					return -1;
				}
				uAcc = (uAcc << 8) | (size_t)in[i + (size_t)k];
			}
			/* Short form required when length fits in 7 bits. */
			if (uAcc <= 127u) {
				return -1;
			}
			/*
			 * Also reject over-long encodings that use more
			 * octets than needed for the value (leading zero
			 * already caught; multi-byte with high zero byte
			 * is the same). Minimal octet count: value must
			 * need all nLenOct bytes — i.e. uAcc >= 1 << (8*(n-1)).
			 * Leading-zero check already enforces that for the
			 * first length octet.
			 */
			cbVal = uAcc;
		}
		i += (size_t)nLenOct;
	}

	cbHdr = i;

	/* Full TLV must fit in the provided buffer (no size_t wrap). */
	if (cbVal > len - cbHdr) {
		return -1;
	}

	*tag = uTag;
	*hdr_len = cbHdr;
	*val_len = cbVal;
	return 0;
}

int __gj_asn1_tlv(const unsigned char *in, size_t len, unsigned *tag,
                  size_t *hdr_len, size_t *val_len)
    __attribute__((alias("gj_asn1_tlv")));
