/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch133: LZ4 block safe decompress.
 * Clean-room freestanding pure C from the public LZ4 block format
 * (https://github.com/lz4/lz4/blob/dev/doc/lz4_Block_format.md).
 * Builds under userspace -msse2. No third-party LZ4 source was copied.
 *
 * Surface (unique symbols):
 *   int gj_lz4_decompress_safe(const unsigned char *src, size_t srcSize,
 *                              unsigned char *dst, size_t dstCapacity);
 *     Decompress one LZ4 block into dst. Returns decompressed size, or -1
 *     on malformed input / bounds violation / NULL args.
 *   __gj_lz4_decompress_safe  (alias)
 *   __libcgj_batch133_marker = "libcgj-batch133"
 *
 * Block format (public):
 *   sequence*  last-sequence
 *   sequence      = token [lit-extra*] literals offset [match-extra*] match
 *   last-sequence = token [lit-extra*] literals
 *   token high nibble = lit len (15 → extras); low nibble = match-4
 *                       (15 → extras). Match copy length = low + 4 + extras.
 *   offset is little-endian u16, must be non-zero and ≤ bytes already out.
 *   Overlapping matches (offset < match len) are valid — byte copy.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch133_marker[] = "libcgj-batch133";

/* ---- helpers ------------------------------------------------------------ */

static uint16_t
b133_u16le(const unsigned char *p)
{
	return (uint16_t)((unsigned)p[0] | ((unsigned)p[1] << 8));
}

/*
 * Extend a length that saturated at 15: sum following bytes until one is
 * less than 255. Updates *ppIn. Returns 0 on success, -1 on truncated
 * input or size_t overflow.
 */
static int
b133_ext_len(const unsigned char **ppIn, const unsigned char *pInEnd,
             size_t *pcb)
{
	const unsigned char *pIn = *ppIn;
	size_t cb = *pcb;

	for (;;) {
		unsigned uB;

		if (pIn >= pInEnd) {
			return -1;
		}
		uB = *pIn++;
		if (cb > (size_t)-1 - (size_t)uB) {
			return -1;
		}
		cb += (size_t)uB;
		if (uB != 255u) {
			break;
		}
	}
	*ppIn = pIn;
	*pcb = cb;
	return 0;
}

/* ---- gj_lz4_decompress_safe -------------------------------------------- */

int
gj_lz4_decompress_safe(const unsigned char *pSrc, size_t cbSrc,
                       unsigned char *pDst, size_t cbDstCap)
{
	const unsigned char *pIn;
	const unsigned char *pInEnd;
	unsigned char *pOut;
	unsigned char *pOutBase;
	unsigned char *pOutEnd;
	size_t cbOut;

	if (pSrc == NULL || pDst == NULL) {
		return -1;
	}
	if (cbSrc == 0u) {
		return 0;
	}

	pIn = pSrc;
	pInEnd = pSrc + cbSrc;
	pOut = pDst;
	pOutBase = pDst;
	pOutEnd = pDst + cbDstCap;

	while (pIn < pInEnd) {
		unsigned uTok;
		size_t cbLit;
		size_t cbMatch;
		uint16_t uOff;
		size_t i;
		const unsigned char *pMatch;

		uTok = *pIn++;
		cbLit = (size_t)(uTok >> 4);
		if (cbLit == 15u) {
			if (b133_ext_len(&pIn, pInEnd, &cbLit) != 0) {
				return -1;
			}
		}

		/* Literals: must fit in remaining src and dst. */
		if ((size_t)(pInEnd - pIn) < cbLit) {
			return -1;
		}
		if ((size_t)(pOutEnd - pOut) < cbLit) {
			return -1;
		}
		for (i = 0u; i < cbLit; i++) {
			pOut[i] = pIn[i];
		}
		pIn += cbLit;
		pOut += cbLit;

		/*
		 * End of block: last sequence is literals-only (offset /
		 * match fields omitted). Detected when compressed input is
		 * fully consumed after the literal run.
		 */
		if (pIn >= pInEnd) {
			break;
		}

		/* Match: 2-byte LE offset, then optional match-length extras. */
		if ((size_t)(pInEnd - pIn) < 2u) {
			return -1;
		}
		uOff = b133_u16le(pIn);
		pIn += 2u;
		if (uOff == 0u) {
			return -1;
		}

		cbMatch = (size_t)(uTok & 0x0fu) + 4u;
		if ((uTok & 0x0fu) == 15u) {
			size_t cbExtra = 0u;

			if (b133_ext_len(&pIn, pInEnd, &cbExtra) != 0) {
				return -1;
			}
			if (cbMatch > (size_t)-1 - cbExtra) {
				return -1;
			}
			cbMatch += cbExtra;
		}

		if ((size_t)(pOut - pOutBase) < (size_t)uOff) {
			return -1;
		}
		if ((size_t)(pOutEnd - pOut) < cbMatch) {
			return -1;
		}

		/* Overlap-safe match copy (offset may be < match length). */
		pMatch = pOut - (size_t)uOff;
		for (i = 0u; i < cbMatch; i++) {
			pOut[i] = pMatch[i];
		}
		pOut += cbMatch;
	}

	cbOut = (size_t)(pOut - pOutBase);
	if (cbOut > (size_t)0x7fffffff) {
		return -1;
	}
	return (int)cbOut;
}

int __gj_lz4_decompress_safe(const unsigned char *src, size_t srcSize,
                             unsigned char *dst, size_t dstCapacity)
    __attribute__((alias("gj_lz4_decompress_safe")));
