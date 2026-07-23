/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch132: Google Snappy raw-block uncompress.
 * Clean-room freestanding pure C from the public Snappy block format
 * (varint uncompressed length + tag stream of literals / back-refs).
 * No third-party Snappy source was copied.
 *
 * Surface (return codes match classic snappy_status):
 *   0 = OK, 1 = invalid input, 2 = buffer too small
 *
 *   int gj_snappy_uncompressed_length(const unsigned char *in, size_t inlen,
 *                                     size_t *outlen);
 *     Parse the leading little-endian base-128 varint (uncompressed size).
 *     *outlen receives that length. Does not decode the tag stream.
 *
 *   int gj_snappy_uncompress(const unsigned char *in, size_t inlen,
 *                            unsigned char *out, size_t *outlen);
 *     Decode a raw Snappy block into out. On entry *outlen is the capacity
 *     of out; on success *outlen is the actual uncompressed length.
 *
 *   __gj_snappy_uncompressed_length / __gj_snappy_uncompress  (aliases)
 *   __libcgj_batch132_marker = "libcgj-batch132"
 *
 * Builds freestanding with -msse2 (SSE2 bulk copy for long literals).
 * Correct for short messages and general blocks; overlapping copies are
 * handled byte-wise (RLE-style backrefs).
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

const char __libcgj_batch132_marker[] = "libcgj-batch132";

/* Return codes (snappy_status-shaped). */
#define B132_OK                 0
#define B132_INVALID_INPUT      1
#define B132_BUFFER_TOO_SMALL   2

/* Snappy uses at most 5 varint bytes for a 32-bit length. */
#define B132_VARINT_MAX_BYTES   5u
/* Practical uncompressed cap (Snappy encode limit is 2^32-1). */
#define B132_MAX_UNCOMPRESSED   0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static void
b132_copy(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i = 0u;

	if (pDst == NULL || pSrc == NULL || cb == 0u) {
		return;
	}

#if defined(__x86_64__) || defined(__i386__)
	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i v = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pSrc + i));
			_mm_storeu_si128((__m128i *)(void *)(pDst + i), v);
			i += 16u;
		}
	}
#endif
	while (i < cb) {
		pDst[i] = pSrc[i];
		i++;
	}
}

/*
 * Overlapping-safe copy from earlier output (offset backref). Must be
 * byte-wise so offset==1 RLE expands correctly.
 */
static void
b132_copy_back(unsigned char *pDst, size_t cbDstOff, size_t cbOffset,
    size_t cbLen)
{
	size_t i;
	size_t cbSrc;

	cbSrc = cbDstOff - cbOffset;
	for (i = 0u; i < cbLen; i++) {
		pDst[cbDstOff + i] = pDst[cbSrc + i];
	}
}

static uint16_t
b132_rd16(const unsigned char *p)
{
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t
b132_rd24(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16);
}

static uint32_t
b132_rd32(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/*
 * Decode little-endian base-128 varint (Snappy uncompressed length).
 * Writes value to *pVal and byte count consumed to *pcbUsed.
 * Returns B132_OK or B132_INVALID_INPUT.
 */
static int
b132_varint(const unsigned char *pIn, size_t cbIn, uint32_t *pVal,
    size_t *pcbUsed)
{
	uint32_t uVal = 0u;
	unsigned uShift = 0u;
	size_t i = 0u;

	if (pIn == NULL || pVal == NULL || pcbUsed == NULL) {
		return B132_INVALID_INPUT;
	}

	while (i < cbIn && i < B132_VARINT_MAX_BYTES) {
		unsigned char uByte = pIn[i];

		i++;
		uVal |= (uint32_t)(uByte & 0x7fu) << uShift;
		if ((uByte & 0x80u) == 0u) {
			*pVal = uVal;
			*pcbUsed = i;
			return B132_OK;
		}
		uShift += 7u;
		/* After 4 continuation bytes, 5th must finish (32-bit). */
		if (uShift >= 28u) {
			/* fall through; next iteration may complete */
		}
	}

	return B132_INVALID_INPUT;
}

/* ---- public API -------------------------------------------------------- */

int
gj_snappy_uncompressed_length(const unsigned char *in, size_t inlen,
    size_t *outlen)
{
	uint32_t uLen;
	size_t cbUsed;
	int nSt;

	if (in == NULL || outlen == NULL || inlen == 0u) {
		return B132_INVALID_INPUT;
	}

	nSt = b132_varint(in, inlen, &uLen, &cbUsed);
	if (nSt != B132_OK) {
		return B132_INVALID_INPUT;
	}

	*outlen = (size_t)uLen;
	return B132_OK;
}

int
gj_snappy_uncompress(const unsigned char *in, size_t inlen,
    unsigned char *out, size_t *outlen)
{
	uint32_t uUncomp;
	size_t cbHdr;
	size_t cbInPos;
	size_t cbOutPos;
	size_t cbCap;
	int nSt;

	if (in == NULL || outlen == NULL || inlen == 0u) {
		return B132_INVALID_INPUT;
	}

	nSt = b132_varint(in, inlen, &uUncomp, &cbHdr);
	if (nSt != B132_OK) {
		return B132_INVALID_INPUT;
	}

	cbCap = *outlen;
	if ((size_t)uUncomp > cbCap) {
		return B132_BUFFER_TOO_SMALL;
	}
	if (uUncomp > 0u && out == NULL) {
		return B132_INVALID_INPUT;
	}

	/* Empty payload: length 0 is valid (no tags required). */
	if (uUncomp == 0u) {
		*outlen = 0u;
		return B132_OK;
	}

	cbInPos = cbHdr;
	cbOutPos = 0u;

	while (cbOutPos < (size_t)uUncomp) {
		unsigned char uTag;
		unsigned uKind;
		size_t cbLit;
		size_t cbOff;
		size_t cbLen;
		unsigned uExtra;

		if (cbInPos >= inlen) {
			return B132_INVALID_INPUT;
		}

		uTag = in[cbInPos];
		cbInPos++;
		uKind = (unsigned)(uTag & 0x03u);

		if (uKind == 0u) {
			/* Literal */
			uExtra = (unsigned)(uTag >> 2);
			if (uExtra < 60u) {
				cbLit = (size_t)uExtra + 1u;
			} else if (uExtra == 60u) {
				if (cbInPos + 1u > inlen) {
					return B132_INVALID_INPUT;
				}
				cbLit = (size_t)in[cbInPos] + 1u;
				cbInPos += 1u;
			} else if (uExtra == 61u) {
				if (cbInPos + 2u > inlen) {
					return B132_INVALID_INPUT;
				}
				cbLit = (size_t)b132_rd16(in + cbInPos) + 1u;
				cbInPos += 2u;
			} else if (uExtra == 62u) {
				if (cbInPos + 3u > inlen) {
					return B132_INVALID_INPUT;
				}
				cbLit = (size_t)b132_rd24(in + cbInPos) + 1u;
				cbInPos += 3u;
			} else {
				/* uExtra == 63 */
				if (cbInPos + 4u > inlen) {
					return B132_INVALID_INPUT;
				}
				cbLit = (size_t)b132_rd32(in + cbInPos) + 1u;
				cbInPos += 4u;
			}

			if (cbLit == 0u ||
			    cbOutPos + cbLit > (size_t)uUncomp ||
			    cbInPos + cbLit > inlen) {
				return B132_INVALID_INPUT;
			}

			b132_copy(out + cbOutPos, in + cbInPos, cbLit);
			cbInPos += cbLit;
			cbOutPos += cbLit;
			continue;
		}

		if (uKind == 1u) {
			/* Copy with 1-byte offset; length in [4, 11] */
			if (cbInPos + 1u > inlen) {
				return B132_INVALID_INPUT;
			}
			cbLen = (size_t)(((uTag >> 2) & 0x07u) + 4u);
			cbOff = ((size_t)(uTag & 0xe0u) << 3) |
			        (size_t)in[cbInPos];
			cbInPos += 1u;
		} else if (uKind == 2u) {
			/* Copy with 2-byte little-endian offset */
			if (cbInPos + 2u > inlen) {
				return B132_INVALID_INPUT;
			}
			cbLen = (size_t)(uTag >> 2) + 1u;
			cbOff = (size_t)b132_rd16(in + cbInPos);
			cbInPos += 2u;
		} else {
			/* uKind == 3: copy with 4-byte little-endian offset */
			if (cbInPos + 4u > inlen) {
				return B132_INVALID_INPUT;
			}
			cbLen = (size_t)(uTag >> 2) + 1u;
			cbOff = (size_t)b132_rd32(in + cbInPos);
			cbInPos += 4u;
		}

		if (cbOff == 0u || cbOff > cbOutPos ||
		    cbOutPos + cbLen > (size_t)uUncomp) {
			return B132_INVALID_INPUT;
		}

		b132_copy_back(out, cbOutPos, cbOff, cbLen);
		cbOutPos += cbLen;
	}

	if (cbOutPos != (size_t)uUncomp) {
		return B132_INVALID_INPUT;
	}

	*outlen = cbOutPos;
	return B132_OK;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_snappy_uncompressed_length(const unsigned char *in, size_t inlen,
    size_t *outlen)
    __attribute__((alias("gj_snappy_uncompressed_length")));

int __gj_snappy_uncompress(const unsigned char *in, size_t inlen,
    unsigned char *out, size_t *outlen)
    __attribute__((alias("gj_snappy_uncompress")));
