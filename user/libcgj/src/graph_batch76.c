/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch76: clean-room DEFLATE inflate (RFC 1951)
 * with zlib (RFC 1950) and gzip (RFC 1952) wrapper detection.
 *
 * Freestanding pure C — integer/pointer only (no SSE). No third-party
 * inflate/zlib source was copied; tables and bit-packing follow the RFCs.
 *
 * Surface:
 *   gj_inflate(const unsigned char *in, size_t inlen,
 *              unsigned char *out, size_t *outlen);
 *   __gj_inflate  (alias)
 *   __libcgj_batch76_marker = "libcgj-batch76"
 *
 * Semantics (zlib uncompress shape):
 *   *outlen is capacity on entry; written size on success exit.
 *   Auto-detect: gzip (1f 8b) → zlib CMF/FLG → else raw DEFLATE.
 *   Blocks: stored (00), fixed Huffman (01), dynamic Huffman (10).
 *   Returns 0 on success; -1 + errno on error.
 *
 * Trailer checks use batch39 crc32_z / adler32_z (extern only).
 * Does NOT define inflate / uncompress / deflate / compress (avoid multi-def).
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch76_marker[] = "libcgj-batch76";

/* batch39 — zlib-shaped checksums (do not redefine) */
unsigned long crc32_z(unsigned long uCrc, const unsigned char *pBuf, size_t cb);
unsigned long adler32_z(unsigned long uAdler, const unsigned char *pBuf, size_t cb);

/* ---- freestanding helpers ---------------------------------------------- */

static void
b76_memset(void *pDst, int nVal, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0; i < cb; i++) {
		p[i] = (unsigned char)nVal;
	}
}

/* ---- bit reader (DEFLATE: LSB-first within the stream) ----------------- */

struct b76_br {
	const unsigned char *pIn;
	size_t cbIn;
	size_t iPos;
	uint32_t uHold;
	unsigned nBits;
	int fErr;
};

static void
b76_br_init(struct b76_br *pBr, const unsigned char *pIn, size_t cbIn)
{
	pBr->pIn = pIn;
	pBr->cbIn = cbIn;
	pBr->iPos = 0;
	pBr->uHold = 0;
	pBr->nBits = 0;
	pBr->fErr = 0;
}

static void
b76_br_align(struct b76_br *pBr)
{
	pBr->uHold >>= (pBr->nBits & 7u);
	pBr->nBits -= (pBr->nBits & 7u);
}

static uint32_t
b76_getbits(struct b76_br *pBr, unsigned nNeed)
{
	uint32_t uVal;

	if (nNeed == 0u) {
		return 0u;
	}
	if (nNeed > 24u) {
		pBr->fErr = 1;
		return 0u;
	}
	while (pBr->nBits < nNeed) {
		if (pBr->iPos >= pBr->cbIn) {
			pBr->fErr = 1;
			return 0u;
		}
		pBr->uHold |= ((uint32_t)pBr->pIn[pBr->iPos++]) << pBr->nBits;
		pBr->nBits += 8u;
	}
	uVal = pBr->uHold & (((uint32_t)1u << nNeed) - 1u);
	pBr->uHold >>= nNeed;
	pBr->nBits -= nNeed;
	return uVal;
}

static int
b76_getbyte(struct b76_br *pBr)
{
	if (pBr->iPos >= pBr->cbIn) {
		pBr->fErr = 1;
		return -1;
	}
	return (int)pBr->pIn[pBr->iPos++];
}

/* ---- canonical Huffman (RFC 1951 §3.2.2) -------------------------------- */

#define B76_MAX_BITS 15u
#define B76_MAX_LITS 288u
#define B76_MAX_DISTS 32u
#define B76_MAX_CLEN 19u

struct b76_huff {
	uint16_t aCount[B76_MAX_BITS + 1u];
	uint16_t aFirst[B76_MAX_BITS + 1u];
	uint16_t aOff[B76_MAX_BITS + 1u];
	uint16_t aSym[B76_MAX_LITS];
	unsigned nMax;
};

/*
 * Build canonical Huffman tables from per-symbol code lengths.
 * Empty tree (all lengths 0) is allowed only for distance when unused.
 */
static int
b76_huff_build(struct b76_huff *pH, const uint8_t *pLen, unsigned nSyms)
{
	unsigned aCount[B76_MAX_BITS + 1u];
	unsigned i;
	unsigned nBits;
	unsigned uCode;
	unsigned nLeft;
	unsigned nMax;
	unsigned aPlace[B76_MAX_BITS + 1u];

	b76_memset(pH, 0, sizeof(*pH));
	b76_memset(aCount, 0, sizeof(aCount));

	nMax = 0u;
	for (i = 0; i < nSyms; i++) {
		if (pLen[i] > B76_MAX_BITS) {
			return -1;
		}
		if (pLen[i] != 0u) {
			aCount[pLen[i]]++;
			if (pLen[i] > nMax) {
				nMax = pLen[i];
			}
		}
	}
	pH->nMax = nMax;
	if (nMax == 0u) {
		/* No codes — decode will fail if used. */
		return 0;
	}

	/* Validate no over-subscription; incomplete trees are RFC-legal. */
	uCode = 0u;
	nLeft = 1u;
	for (nBits = 1u; nBits <= B76_MAX_BITS; nBits++) {
		nLeft <<= 1;
		if (nLeft < aCount[nBits]) {
			return -1;
		}
		nLeft -= aCount[nBits];
		uCode = (uCode + aCount[nBits - 1u]) << 1;
		pH->aFirst[nBits] = (uint16_t)uCode;
		pH->aCount[nBits] = (uint16_t)aCount[nBits];
	}

	/* Symbol table offsets by length. */
	pH->aOff[0] = 0;
	pH->aOff[1] = 0;
	for (nBits = 1u; nBits < B76_MAX_BITS; nBits++) {
		pH->aOff[nBits + 1u] =
		    (uint16_t)(pH->aOff[nBits] + aCount[nBits]);
	}

	for (nBits = 0u; nBits <= B76_MAX_BITS; nBits++) {
		aPlace[nBits] = pH->aOff[nBits];
	}
	for (i = 0; i < nSyms; i++) {
		nBits = pLen[i];
		if (nBits != 0u) {
			pH->aSym[aPlace[nBits]++] = (uint16_t)i;
		}
	}
	return 0;
}

static int
b76_huff_decode(struct b76_br *pBr, const struct b76_huff *pH)
{
	unsigned uCode;
	unsigned nBits;
	unsigned nIdx;

	if (pH->nMax == 0u) {
		pBr->fErr = 1;
		return -1;
	}

	uCode = 0u;
	for (nBits = 1u; nBits <= pH->nMax; nBits++) {
		uCode = (uCode << 1) | b76_getbits(pBr, 1u);
		if (pBr->fErr) {
			return -1;
		}
		if (pH->aCount[nBits] == 0u) {
			continue;
		}
		if (uCode >= (unsigned)pH->aFirst[nBits] &&
		    uCode < (unsigned)pH->aFirst[nBits] +
		                (unsigned)pH->aCount[nBits]) {
			nIdx = (unsigned)pH->aOff[nBits] +
			       (uCode - (unsigned)pH->aFirst[nBits]);
			return (int)pH->aSym[nIdx];
		}
	}
	pBr->fErr = 1;
	return -1;
}

/* ---- length / distance base + extra bits (RFC 1951) -------------------- */

static const uint16_t s_b76_len_base[29] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
};
static const uint8_t s_b76_len_extra[29] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};
static const uint16_t s_b76_dist_base[30] = {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289,
    16385, 24577
};
static const uint8_t s_b76_dist_extra[30] = {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};

/* Code-length alphabet order (RFC 1951 §3.2.7) */
static const uint8_t s_b76_clen_order[19] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/* ---- inflate core ------------------------------------------------------ */

struct b76_out {
	unsigned char *pOut;
	size_t cbCap;
	size_t cbUsed;
};

static int
b76_emit(struct b76_out *pO, unsigned char ch)
{
	if (pO->cbUsed >= pO->cbCap) {
		return -1;
	}
	pO->pOut[pO->cbUsed++] = ch;
	return 0;
}

static int
b76_copy_match(struct b76_out *pO, unsigned uDist, unsigned uLen)
{
	size_t iSrc;
	unsigned i;

	if (uDist == 0u || uDist > pO->cbUsed) {
		return -1;
	}
	iSrc = pO->cbUsed - (size_t)uDist;
	for (i = 0; i < uLen; i++) {
		if (b76_emit(pO, pO->pOut[iSrc + i]) != 0) {
			return -1;
		}
	}
	return 0;
}

static int
b76_inflate_codes(struct b76_br *pBr, struct b76_out *pO,
                  const struct b76_huff *pLit, const struct b76_huff *pDist)
{
	for (;;) {
		int nSym;
		unsigned uLen;
		unsigned uDist;
		unsigned uExtra;
		int nDsym;

		nSym = b76_huff_decode(pBr, pLit);
		if (nSym < 0 || pBr->fErr) {
			return -1;
		}
		if (nSym < 256) {
			if (b76_emit(pO, (unsigned char)nSym) != 0) {
				return -1;
			}
			continue;
		}
		if (nSym == 256) {
			return 0; /* end of block */
		}
		if (nSym > 285) {
			return -1;
		}
		/* Length code 257..285 */
		nSym -= 257;
		uLen = s_b76_len_base[nSym];
		uExtra = s_b76_len_extra[nSym];
		if (uExtra != 0u) {
			uLen += b76_getbits(pBr, uExtra);
			if (pBr->fErr) {
				return -1;
			}
		}

		nDsym = b76_huff_decode(pBr, pDist);
		if (nDsym < 0 || nDsym > 29 || pBr->fErr) {
			return -1;
		}
		uDist = s_b76_dist_base[nDsym];
		uExtra = s_b76_dist_extra[nDsym];
		if (uExtra != 0u) {
			uDist += b76_getbits(pBr, uExtra);
			if (pBr->fErr) {
				return -1;
			}
		}
		if (b76_copy_match(pO, uDist, uLen) != 0) {
			return -1;
		}
	}
}

static int
b76_fixed_trees(struct b76_huff *pLit, struct b76_huff *pDist)
{
	uint8_t aLen[B76_MAX_LITS];
	uint8_t aDist[B76_MAX_DISTS];
	unsigned i;

	for (i = 0; i <= 143u; i++) {
		aLen[i] = 8;
	}
	for (i = 144u; i <= 255u; i++) {
		aLen[i] = 9;
	}
	for (i = 256u; i <= 279u; i++) {
		aLen[i] = 7;
	}
	for (i = 280u; i <= 287u; i++) {
		aLen[i] = 8;
	}
	for (i = 0; i < B76_MAX_DISTS; i++) {
		aDist[i] = 5;
	}
	if (b76_huff_build(pLit, aLen, B76_MAX_LITS) != 0) {
		return -1;
	}
	if (b76_huff_build(pDist, aDist, B76_MAX_DISTS) != 0) {
		return -1;
	}
	return 0;
}

static int
b76_dynamic_trees(struct b76_br *pBr, struct b76_huff *pLit,
                  struct b76_huff *pDist)
{
	unsigned nHlit;
	unsigned nHdist;
	unsigned nHclen;
	unsigned i;
	unsigned nTotal;
	uint8_t aClen[B76_MAX_CLEN];
	uint8_t aLens[B76_MAX_LITS + B76_MAX_DISTS];
	struct b76_huff hClen;
	unsigned nPrev;

	nHlit = b76_getbits(pBr, 5u) + 257u;
	nHdist = b76_getbits(pBr, 5u) + 1u;
	nHclen = b76_getbits(pBr, 4u) + 4u;
	if (pBr->fErr || nHlit > B76_MAX_LITS || nHdist > B76_MAX_DISTS) {
		return -1;
	}

	b76_memset(aClen, 0, sizeof(aClen));
	for (i = 0; i < nHclen; i++) {
		aClen[s_b76_clen_order[i]] = (uint8_t)b76_getbits(pBr, 3u);
		if (pBr->fErr) {
			return -1;
		}
	}
	if (b76_huff_build(&hClen, aClen, B76_MAX_CLEN) != 0) {
		return -1;
	}

	nTotal = nHlit + nHdist;
	b76_memset(aLens, 0, sizeof(aLens));
	nPrev = 0u;
	i = 0u;
	while (i < nTotal) {
		int nSym;
		unsigned nRep;
		unsigned nVal;

		nSym = b76_huff_decode(pBr, &hClen);
		if (nSym < 0 || pBr->fErr) {
			return -1;
		}
		if (nSym < 16) {
			aLens[i++] = (uint8_t)nSym;
			nPrev = (unsigned)nSym;
			continue;
		}
		if (nSym == 16) {
			nRep = b76_getbits(pBr, 2u) + 3u;
			nVal = nPrev;
		} else if (nSym == 17) {
			nRep = b76_getbits(pBr, 3u) + 3u;
			nVal = 0u;
		} else if (nSym == 18) {
			nRep = b76_getbits(pBr, 7u) + 11u;
			nVal = 0u;
		} else {
			return -1;
		}
		if (pBr->fErr || i + nRep > nTotal) {
			return -1;
		}
		while (nRep-- > 0u) {
			aLens[i++] = (uint8_t)nVal;
		}
		if (nSym == 16) {
			/* nPrev already set */
		} else {
			nPrev = 0u;
		}
	}

	if (b76_huff_build(pLit, aLens, nHlit) != 0) {
		return -1;
	}
	if (b76_huff_build(pDist, aLens + nHlit, nHdist) != 0) {
		return -1;
	}
	return 0;
}

static int
b76_stored_block(struct b76_br *pBr, struct b76_out *pO)
{
	unsigned uLen;
	unsigned uNlen;
	unsigned i;
	int nB;

	b76_br_align(pBr);
	uLen = (unsigned)b76_getbits(pBr, 16u);
	uNlen = (unsigned)b76_getbits(pBr, 16u);
	if (pBr->fErr) {
		return -1;
	}
	if ((uLen ^ 0xffffu) != uNlen) {
		return -1;
	}
	/* Stored payload is byte-aligned; pull from input directly. */
	if (pBr->nBits != 0u) {
		/* After align, hold should be empty. */
		return -1;
	}
	if (pBr->iPos + (size_t)uLen > pBr->cbIn) {
		return -1;
	}
	for (i = 0; i < uLen; i++) {
		nB = b76_getbyte(pBr);
		if (nB < 0) {
			return -1;
		}
		if (b76_emit(pO, (unsigned char)nB) != 0) {
			return -1;
		}
	}
	return 0;
}

static int
b76_inflate_raw(struct b76_br *pBr, struct b76_out *pO)
{
	struct b76_huff hLit;
	struct b76_huff hDist;
	unsigned fBfinal;
	unsigned uBtype;

	do {
		fBfinal = b76_getbits(pBr, 1u);
		uBtype = b76_getbits(pBr, 2u);
		if (pBr->fErr) {
			return -1;
		}
		if (uBtype == 0u) {
			if (b76_stored_block(pBr, pO) != 0) {
				return -1;
			}
		} else if (uBtype == 1u) {
			if (b76_fixed_trees(&hLit, &hDist) != 0) {
				return -1;
			}
			if (b76_inflate_codes(pBr, pO, &hLit, &hDist) != 0) {
				return -1;
			}
		} else if (uBtype == 2u) {
			if (b76_dynamic_trees(pBr, &hLit, &hDist) != 0) {
				return -1;
			}
			if (b76_inflate_codes(pBr, pO, &hLit, &hDist) != 0) {
				return -1;
			}
		} else {
			/* reserved */
			return -1;
		}
	} while (fBfinal == 0u);

	return 0;
}

/* ---- wrappers: zlib / gzip / raw --------------------------------------- */

static int
b76_is_zlib(const unsigned char *pIn, size_t cbIn)
{
	unsigned uCmf;
	unsigned uFlg;
	unsigned uCheck;

	if (cbIn < 2u) {
		return 0;
	}
	uCmf = pIn[0];
	uFlg = pIn[1];
	if ((uCmf & 0x0fu) != 8u) {
		return 0; /* only deflate */
	}
	if ((uCmf >> 4) > 7u) {
		return 0;
	}
	uCheck = (uCmf << 8) | uFlg;
	if ((uCheck % 31u) != 0u) {
		return 0;
	}
	return 1;
}

static int
b76_is_gzip(const unsigned char *pIn, size_t cbIn)
{
	if (cbIn < 10u) {
		return 0;
	}
	if (pIn[0] != 0x1fu || pIn[1] != 0x8bu) {
		return 0;
	}
	if (pIn[2] != 8u) {
		return 0; /* CM = deflate */
	}
	return 1;
}

static int
b76_skip_gzip_header(const unsigned char *pIn, size_t cbIn, size_t *pOff)
{
	unsigned uFlg;
	size_t i;

	if (cbIn < 10u) {
		return -1;
	}
	uFlg = pIn[3];
	i = 10u;

	/* FEXTRA */
	if ((uFlg & 0x04u) != 0u) {
		unsigned uXlen;

		if (i + 2u > cbIn) {
			return -1;
		}
		uXlen = (unsigned)pIn[i] | ((unsigned)pIn[i + 1u] << 8);
		i += 2u + (size_t)uXlen;
		if (i > cbIn) {
			return -1;
		}
	}
	/* FNAME */
	if ((uFlg & 0x08u) != 0u) {
		while (i < cbIn && pIn[i] != 0u) {
			i++;
		}
		if (i >= cbIn) {
			return -1;
		}
		i++; /* NUL */
	}
	/* FCOMMENT */
	if ((uFlg & 0x10u) != 0u) {
		while (i < cbIn && pIn[i] != 0u) {
			i++;
		}
		if (i >= cbIn) {
			return -1;
		}
		i++;
	}
	/* FHCRC */
	if ((uFlg & 0x02u) != 0u) {
		if (i + 2u > cbIn) {
			return -1;
		}
		i += 2u;
	}
	*pOff = i;
	return 0;
}

/*
 * gj_inflate — decompress zlib, gzip, or raw DEFLATE into caller buffer.
 *
 * @pIn     compressed bytes
 * @cbIn    compressed length
 * @pOut    destination buffer
 * @pcbOut  in: capacity; out: bytes written (on success)
 * @return  0 success, -1 error (errno set)
 */
int
gj_inflate(const unsigned char *pIn, size_t cbIn, unsigned char *pOut,
           size_t *pcbOut)
{
	struct b76_br br;
	struct b76_out out;
	size_t iOff;
	size_t cbCap;
	int fGzip;
	int fZlib;
	unsigned long uAdler;
	unsigned long uCrc;
	uint32_t uExpect;
	uint32_t uIsize;

	if (pcbOut == NULL) {
		errno = EINVAL;
		return -1;
	}
	cbCap = *pcbOut;
	if (pIn == NULL || (pOut == NULL && cbCap != 0u)) {
		errno = EINVAL;
		return -1;
	}
	if (cbIn == 0u) {
		errno = EINVAL;
		return -1;
	}

	fGzip = b76_is_gzip(pIn, cbIn);
	fZlib = 0;
	iOff = 0u;

	if (fGzip) {
		if (b76_skip_gzip_header(pIn, cbIn, &iOff) != 0) {
			errno = EINVAL;
			return -1;
		}
	} else if (b76_is_zlib(pIn, cbIn)) {
		unsigned uFlg;

		fZlib = 1;
		uFlg = pIn[1];
		iOff = 2u;
		if ((uFlg & 0x20u) != 0u) {
			/* preset dictionary — skip 4-byte DICTID (no dict support) */
			if (cbIn < 6u) {
				errno = EINVAL;
				return -1;
			}
			iOff = 6u;
			/* Reject: we have no dictionary. */
			errno = EINVAL;
			return -1;
		}
	}

	if (iOff >= cbIn) {
		errno = EINVAL;
		return -1;
	}

	b76_br_init(&br, pIn + iOff, cbIn - iOff);
	out.pOut = pOut;
	out.cbCap = cbCap;
	out.cbUsed = 0u;

	if (b76_inflate_raw(&br, &out) != 0) {
		errno = EIO;
		return -1;
	}

	/* Trailer verification for wrappers. */
	if (fZlib) {
		/* Adler-32, big-endian, after bit-aligned end of stream. */
		b76_br_align(&br);
		if (br.nBits != 0u) {
			/* should be byte-aligned after align */
			br.uHold = 0;
			br.nBits = 0;
		}
		if (br.iPos + 4u > br.cbIn) {
			errno = EIO;
			return -1;
		}
		uExpect = ((uint32_t)br.pIn[br.iPos] << 24) |
		          ((uint32_t)br.pIn[br.iPos + 1u] << 16) |
		          ((uint32_t)br.pIn[br.iPos + 2u] << 8) |
		          ((uint32_t)br.pIn[br.iPos + 3u]);
		uAdler = adler32_z(1ul, pOut, out.cbUsed);
		if ((uint32_t)uAdler != uExpect) {
			errno = EIO;
			return -1;
		}
	} else if (fGzip) {
		b76_br_align(&br);
		if (br.nBits != 0u) {
			br.uHold = 0;
			br.nBits = 0;
		}
		if (br.iPos + 8u > br.cbIn) {
			errno = EIO;
			return -1;
		}
		uExpect = ((uint32_t)br.pIn[br.iPos]) |
		          ((uint32_t)br.pIn[br.iPos + 1u] << 8) |
		          ((uint32_t)br.pIn[br.iPos + 2u] << 16) |
		          ((uint32_t)br.pIn[br.iPos + 3u] << 24);
		uIsize = ((uint32_t)br.pIn[br.iPos + 4u]) |
		         ((uint32_t)br.pIn[br.iPos + 5u] << 8) |
		         ((uint32_t)br.pIn[br.iPos + 6u] << 16) |
		         ((uint32_t)br.pIn[br.iPos + 7u] << 24);
		uCrc = crc32_z(0ul, pOut, out.cbUsed);
		if ((uint32_t)uCrc != uExpect) {
			errno = EIO;
			return -1;
		}
		if ((uint32_t)(out.cbUsed & 0xfffffffful) != uIsize) {
			errno = EIO;
			return -1;
		}
	}

	*pcbOut = out.cbUsed;
	return 0;
}

int __gj_inflate(const unsigned char *pIn, size_t cbIn, unsigned char *pOut,
                 size_t *pcbOut)
    __attribute__((alias("gj_inflate")));
