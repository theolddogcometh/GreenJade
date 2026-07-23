/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch99: clean-room DEFLATE deflate (RFC 1951)
 * with zlib wrapper (RFC 1950). Counterpart to batch76 gj_inflate.
 *
 * Freestanding pure C — integer/pointer only (no SSE required; still
 * builds under userspace -msse2). No third-party deflate/zlib source
 * was copied; tables and bit-packing follow the RFCs.
 *
 * Soft deepen: stored (level 0) vs fixed Huffman paths, zlib CMF/FLG +
 * Adler-32 trailer, capacity ENOSPC reject, level clamp.
 *
 * Surface:
 *   gj_deflate(const unsigned char *in, size_t inlen,
 *              unsigned char *out, size_t *outlen, int level);
 *   __gj_deflate  (alias)
 *   __libcgj_batch99_marker = "libcgj-batch99"
 *
 * Semantics (zlib compress shape):
 *   *outlen is capacity on entry; written size on success exit.
 *   Output is always zlib-wrapped DEFLATE (CMF/FLG + raw blocks + Adler-32).
 *   level <= 0 : stored blocks only (BTYPE=00) — bring-up / level 0.
 *   level >= 1 : fixed Huffman (BTYPE=01), literals only (no LZ matches).
 *   Returns 0 on success; -1 + errno on error.
 *
 * Adler-32 trailer uses batch39 adler32_z (extern only).
 * Does NOT define deflate / compress / inflate / uncompress (avoid multi-def).
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch99_marker[] = "libcgj-batch99";

/* batch39 — zlib-shaped checksum (do not redefine) */
unsigned long adler32_z(unsigned long uAdler, const unsigned char *pBuf,
                        size_t cb);

/* Max payload of one stored block (RFC 1951). */
#define B99_STORED_MAX 65535u

/* ---- freestanding helpers ---------------------------------------------- */

static void
b99_memcpy(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

/* Reverse the low nBits of v (for MSB-first Huffman into LSB-first stream). */
static unsigned
b99_bitrev(unsigned uVal, unsigned nBits)
{
	unsigned uR = 0u;
	unsigned i;

	for (i = 0u; i < nBits; i++) {
		uR = (uR << 1) | (uVal & 1u);
		uVal >>= 1;
	}
	return uR;
}

/* ---- bit writer (DEFLATE: LSB-first within the byte stream) ------------ */

struct b99_bw {
	unsigned char *pOut;
	size_t cbCap;
	size_t iPos;
	uint32_t uHold;
	unsigned nBits;
	int fErr;
};

static void
b99_bw_init(struct b99_bw *pBw, unsigned char *pOut, size_t cbCap)
{
	pBw->pOut = pOut;
	pBw->cbCap = cbCap;
	pBw->iPos = 0u;
	pBw->uHold = 0u;
	pBw->nBits = 0u;
	pBw->fErr = 0;
}

static void
b99_putbits(struct b99_bw *pBw, uint32_t uVal, unsigned nNeed)
{
	if (pBw->fErr) {
		return;
	}
	if (nNeed == 0u) {
		return;
	}
	if (nNeed > 24u) {
		pBw->fErr = 1;
		return;
	}
	pBw->uHold |= (uVal & (((uint32_t)1u << nNeed) - 1u)) << pBw->nBits;
	pBw->nBits += nNeed;
	while (pBw->nBits >= 8u) {
		if (pBw->iPos >= pBw->cbCap) {
			pBw->fErr = 1;
			return;
		}
		pBw->pOut[pBw->iPos++] = (unsigned char)(pBw->uHold & 0xffu);
		pBw->uHold >>= 8;
		pBw->nBits -= 8u;
	}
}

/* Pad to next byte boundary with zero bits. */
static void
b99_align(struct b99_bw *pBw)
{
	unsigned nPad;

	if (pBw->fErr) {
		return;
	}
	nPad = pBw->nBits & 7u;
	if (nPad != 0u) {
		b99_putbits(pBw, 0u, 8u - nPad);
	}
}

/* Flush partial final byte (if any). */
static int
b99_flush(struct b99_bw *pBw)
{
	if (pBw->fErr) {
		return -1;
	}
	if (pBw->nBits > 0u) {
		if (pBw->iPos >= pBw->cbCap) {
			pBw->fErr = 1;
			return -1;
		}
		pBw->pOut[pBw->iPos++] = (unsigned char)(pBw->uHold & 0xffu);
		pBw->uHold = 0u;
		pBw->nBits = 0u;
	}
	return 0;
}

static int
b99_putbyte(struct b99_bw *pBw, unsigned char ch)
{
	if (pBw->fErr) {
		return -1;
	}
	/* Caller must have byte-aligned the stream. */
	if (pBw->nBits != 0u) {
		pBw->fErr = 1;
		return -1;
	}
	if (pBw->iPos >= pBw->cbCap) {
		pBw->fErr = 1;
		return -1;
	}
	pBw->pOut[pBw->iPos++] = ch;
	return 0;
}

/* ---- zlib header (RFC 1950) -------------------------------------------- */

/*
 * CMF = 0x78: CM=8 (deflate), CINFO=7 (32 KiB window).
 * FLEVEL bits of FLG from level; FDICT=0; FCHECK so that
 * (CMF<<8|FLG) % 31 == 0.
 */
static void
b99_write_zlib_hdr(struct b99_bw *pBw, int nLevel)
{
	unsigned uCmf = 0x78u;
	unsigned uFlevel;
	unsigned uFlg;
	unsigned uCheck;

	if (nLevel <= 1) {
		uFlevel = 0u; /* fastest */
	} else if (nLevel <= 5) {
		uFlevel = 1u;
	} else if (nLevel <= 7) {
		uFlevel = 2u; /* default */
	} else {
		uFlevel = 3u; /* maximum */
	}
	uFlg = (uFlevel << 6);
	uCheck = (uCmf << 8) | uFlg;
	uFlg |= (31u - (uCheck % 31u)) % 31u;

	(void)b99_putbyte(pBw, (unsigned char)uCmf);
	(void)b99_putbyte(pBw, (unsigned char)uFlg);
}

static int
b99_write_adler(struct b99_bw *pBw, const unsigned char *pIn, size_t cbIn)
{
	unsigned long uAdler;
	uint32_t uA;

	uAdler = adler32_z(1ul, pIn, cbIn);
	uA = (uint32_t)uAdler;
	/* Big-endian Adler-32 trailer. */
	if (b99_putbyte(pBw, (unsigned char)((uA >> 24) & 0xffu)) != 0 ||
	    b99_putbyte(pBw, (unsigned char)((uA >> 16) & 0xffu)) != 0 ||
	    b99_putbyte(pBw, (unsigned char)((uA >> 8) & 0xffu)) != 0 ||
	    b99_putbyte(pBw, (unsigned char)(uA & 0xffu)) != 0) {
		return -1;
	}
	return 0;
}

/* ---- stored blocks (BTYPE=00) ------------------------------------------ */

static int
b99_emit_stored(struct b99_bw *pBw, const unsigned char *pIn, size_t cbIn)
{
	size_t iOff = 0u;

	/* Always emit at least one final empty/non-empty stored block. */
	do {
		size_t cbChunk;
		unsigned uLen;
		unsigned uNlen;
		unsigned fFinal;

		cbChunk = cbIn - iOff;
		if (cbChunk > (size_t)B99_STORED_MAX) {
			cbChunk = (size_t)B99_STORED_MAX;
		}
		fFinal = (iOff + cbChunk >= cbIn) ? 1u : 0u;
		uLen = (unsigned)cbChunk;
		uNlen = uLen ^ 0xffffu;

		/* BFINAL + BTYPE=00 */
		b99_putbits(pBw, fFinal, 1u);
		b99_putbits(pBw, 0u, 2u);
		b99_align(pBw);
		if (pBw->fErr) {
			return -1;
		}

		/* LEN / NLEN little-endian (LSB-first putbits == LE wire). */
		b99_putbits(pBw, (uint32_t)uLen, 16u);
		b99_putbits(pBw, (uint32_t)uNlen, 16u);
		if (pBw->fErr) {
			return -1;
		}
		/* After 16+16 bits from aligned state, still byte-aligned. */
		if (pBw->nBits != 0u) {
			pBw->fErr = 1;
			return -1;
		}

		if (cbChunk > 0u) {
			if (pBw->iPos + cbChunk > pBw->cbCap) {
				pBw->fErr = 1;
				return -1;
			}
			b99_memcpy(pBw->pOut + pBw->iPos, pIn + iOff, cbChunk);
			pBw->iPos += cbChunk;
		}

		iOff += cbChunk;
		/* Empty input: one final empty stored block, then stop. */
		if (cbIn == 0u) {
			break;
		}
	} while (iOff < cbIn);

	return pBw->fErr ? -1 : 0;
}

/* ---- fixed Huffman (BTYPE=01), literals only --------------------------- */

/*
 * Fixed lit/length codes (RFC 1951 §3.2.6):
 *   0..143   : 8 bits, codes 00110000..10111111  (0x30..0xBF)
 *   144..255 : 9 bits, codes 110010000..111111111 (0x190..0x1FF)
 *   256..279 : 7 bits, codes 0000000..0010111
 *   280..287 : 8 bits, codes 11000000..11000111  (0xC0..0xC7)
 * Codes are written MSB-first into an LSB-first bit stream → bit-reverse.
 */
static void
b99_fixed_sym(struct b99_bw *pBw, unsigned uSym)
{
	unsigned uCode;
	unsigned nBits;

	if (uSym <= 143u) {
		uCode = 0x30u + uSym;
		nBits = 8u;
	} else if (uSym <= 255u) {
		uCode = 0x190u + (uSym - 144u);
		nBits = 9u;
	} else if (uSym <= 279u) {
		uCode = uSym - 256u;
		nBits = 7u;
	} else if (uSym <= 287u) {
		uCode = 0xc0u + (uSym - 280u);
		nBits = 8u;
	} else {
		pBw->fErr = 1;
		return;
	}
	b99_putbits(pBw, (uint32_t)b99_bitrev(uCode, nBits), nBits);
}

static int
b99_emit_fixed(struct b99_bw *pBw, const unsigned char *pIn, size_t cbIn)
{
	size_t i;

	/* Single final fixed-Huffman block. */
	b99_putbits(pBw, 1u, 1u); /* BFINAL */
	b99_putbits(pBw, 1u, 2u); /* BTYPE = 01 */
	if (pBw->fErr) {
		return -1;
	}

	for (i = 0; i < cbIn; i++) {
		b99_fixed_sym(pBw, (unsigned)pIn[i]);
		if (pBw->fErr) {
			return -1;
		}
	}
	/* End-of-block symbol 256. */
	b99_fixed_sym(pBw, 256u);
	if (pBw->fErr) {
		return -1;
	}
	return 0;
}

/* ---- public API -------------------------------------------------------- */

/*
 * gj_deflate — compress to zlib-wrapped DEFLATE into caller buffer.
 *
 * @pIn     input bytes (may be NULL only when cbIn == 0)
 * @cbIn    input length
 * @pOut    destination buffer
 * @pcbOut  in: capacity; out: bytes written (on success)
 * @nLevel  0 → stored blocks; >=1 → fixed Huffman (literals only)
 * @return  0 success, -1 error (errno set)
 */
int
gj_deflate(const unsigned char *pIn, size_t cbIn, unsigned char *pOut,
           size_t *pcbOut, int nLevel)
{
	struct b99_bw bw;
	size_t cbCap;

	if (pcbOut == NULL) {
		errno = EINVAL;
		return -1;
	}
	cbCap = *pcbOut;
	if (pIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (pOut == NULL && cbCap != 0u) {
		errno = EINVAL;
		return -1;
	}
	if (pOut == NULL) {
		/* Zero capacity and NULL out — cannot write zlib header. */
		errno = ENOSPC;
		return -1;
	}

	/* Negative levels treated as store-only (zlib-ish). */
	if (nLevel < 0) {
		nLevel = 0;
	}

	b99_bw_init(&bw, pOut, cbCap);
	b99_write_zlib_hdr(&bw, nLevel);
	if (bw.fErr) {
		errno = ENOSPC;
		return -1;
	}

	if (nLevel <= 0) {
		if (b99_emit_stored(&bw, pIn, cbIn) != 0) {
			errno = ENOSPC;
			return -1;
		}
	} else {
		if (b99_emit_fixed(&bw, pIn, cbIn) != 0) {
			errno = ENOSPC;
			return -1;
		}
	}

	if (b99_flush(&bw) != 0) {
		errno = ENOSPC;
		return -1;
	}

	/* Adler-32 over uncompressed input (RFC 1950). */
	if (b99_write_adler(&bw, pIn, cbIn) != 0) {
		errno = ENOSPC;
		return -1;
	}

	*pcbOut = bw.iPos;
	return 0;
}

int __gj_deflate(const unsigned char *pIn, size_t cbIn, unsigned char *pOut,
                 size_t *pcbOut, int nLevel)
    __attribute__((alias("gj_deflate")));
