/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch123: ZIP End-of-Central-Directory (EOCD)
 * locate/parse and Central Directory (CD) entry walk — metadata only
 * (no local-file extract / inflate / full unzip).
 *
 * Clean-room freestanding pure C from the public APPNOTE.TXT layout
 * (PKWARE ZIP). No third-party unzip source was copied.
 *
 * Surface (0 success, -1 error with errno):
 *   int gj_zip_eocd_find(const void *buf, size_t len, size_t *eocd_off);
 *     Scan buf[0..len) from the end for EOCD signature 0x06054b50
 *     (PK\\x05\\x06). Max comment length 65535 → search window
 *     min(len, 22+65535). Validates comment length field against
 *     remaining bytes. *eocd_off = byte offset of EOCD in buf.
 *   int gj_zip_eocd_parse(const void *eocd, size_t len,
 *                         unsigned *entries, uint32_t *cd_off,
 *                         uint32_t *cd_size);
 *     Parse a buffer that starts at an EOCD (min 22 bytes). Outputs
 *     total CD records, CD size, and CD start offset (classic 32-bit
 *     fields; Zip64 sentinels 0xFFFF / 0xFFFFFFFF are returned as-is).
 *   int gj_zip_cd_entry(const void *cd, size_t len, unsigned idx,
 *                       char name[256], uint32_t *comp_size,
 *                       uint32_t *uncomp_size, uint16_t *method);
 *     Walk the Central Directory buffer to the idx-th entry (0-based).
 *     Fills compression method, sizes, and NUL-terminated name (capped
 *     at 255 chars + NUL; longer names are truncated in name[]).
 *   __libcgj_batch123_marker = "libcgj-batch123"
 *
 * Builds freestanding with -msse2 (SSE2 bulk name/byte copy when cb>=16).
 */

#include <errno.h>
#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch123_marker[] = "libcgj-batch123";

/* APPNOTE signatures (little-endian uint32 on LE hosts). */
#define B123_SIG_EOCD  0x06054b50u /* PK\\x05\\x06 */
#define B123_SIG_CDFH  0x02014b50u /* PK\\x01\\x02 */

#define B123_EOCD_MIN  22u
#define B123_CDFH_MIN  46u
#define B123_COMMENT_MAX 65535u

/* ---- freestanding LE loads + byte copy --------------------------------- */

static uint16_t
b123_rd16(const unsigned char *p)
{
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t
b123_rd32(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static void
b123_copy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD = (unsigned char *)pDst;
	const unsigned char *pS = (const unsigned char *)pSrc;
	size_t i = 0u;

	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i v = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pS + i));
			_mm_storeu_si128((__m128i *)(void *)(pD + i), v);
			i += 16u;
		}
	}
	while (i < cb) {
		pD[i] = pS[i];
		i++;
	}
}

/* ---- EOCD find --------------------------------------------------------- */

/*
 * Locate EOCD in a ZIP image buffer. Returns 0 and *peocd_off on success.
 */
int
gj_zip_eocd_find(const void *pBuf, size_t cbLen, size_t *pcbEocdOff)
{
	const unsigned char *pB;
	size_t cbWin;
	size_t iOff;

	if (pBuf == NULL || pcbEocdOff == NULL || cbLen < B123_EOCD_MIN) {
		errno = EINVAL;
		return -1;
	}

	pB = (const unsigned char *)pBuf;
	/* EOCD is near the end; comment ≤ 65535. */
	cbWin = B123_EOCD_MIN + (size_t)B123_COMMENT_MAX;
	if (cbWin > cbLen) {
		cbWin = cbLen;
	}

	/*
	 * Scan from the latest possible EOCD start toward the earliest
	 * in the window so a trailing-comment false positive is less
	 * likely than a real EOCD (APPNOTE: last 22+comment bytes).
	 */
	iOff = cbLen - B123_EOCD_MIN;
	for (;;) {
		size_t iRel = iOff - (cbLen - cbWin);
		const unsigned char *pCand = pB + iOff;
		uint32_t uSig;
		uint16_t u16Comment;
		size_t cbTail;

		uSig = b123_rd32(pCand);
		if (uSig == B123_SIG_EOCD) {
			u16Comment = b123_rd16(pCand + 20);
			cbTail = cbLen - iOff;
			/* Fixed header + comment must exhaust the buffer. */
			if (cbTail >= B123_EOCD_MIN &&
			    (size_t)u16Comment == cbTail - B123_EOCD_MIN) {
				*pcbEocdOff = iOff;
				return 0;
			}
		}

		if (iRel == 0u) {
			break;
		}
		iOff--;
	}

	errno = ENOENT;
	return -1;
}

/* ---- EOCD parse -------------------------------------------------------- */

int
gj_zip_eocd_parse(const void *pEocd, size_t cbLen, unsigned *pcEntries,
    uint32_t *puCdOff, uint32_t *puCdSize)
{
	const unsigned char *pE;
	uint32_t uSig;
	uint16_t u16Entries;
	uint16_t u16Comment;
	uint32_t u32CdSize;
	uint32_t u32CdOff;

	if (pEocd == NULL || pcEntries == NULL || puCdOff == NULL ||
	    puCdSize == NULL || cbLen < B123_EOCD_MIN) {
		errno = EINVAL;
		return -1;
	}

	pE = (const unsigned char *)pEocd;
	uSig = b123_rd32(pE);
	if (uSig != B123_SIG_EOCD) {
		errno = EINVAL;
		return -1;
	}

	u16Comment = b123_rd16(pE + 20);
	if ((size_t)u16Comment > cbLen - B123_EOCD_MIN) {
		errno = EINVAL;
		return -1;
	}

	/* total number of central directory records @ +10 */
	u16Entries = b123_rd16(pE + 10);
	/* size of the central directory @ +12 */
	u32CdSize = b123_rd32(pE + 12);
	/* offset of start of central directory @ +16 */
	u32CdOff = b123_rd32(pE + 16);

	*pcEntries = (unsigned)u16Entries;
	*puCdSize = u32CdSize;
	*puCdOff = u32CdOff;
	return 0;
}

/* ---- Central Directory entry ------------------------------------------- */

int
gj_zip_cd_entry(const void *pCd, size_t cbLen, unsigned uIdx, char szName[256],
    uint32_t *puCompSize, uint32_t *puUncompSize, uint16_t *puMethod)
{
	const unsigned char *pBase;
	const unsigned char *pCur;
	const unsigned char *pEnd;
	unsigned uCur;

	if (pCd == NULL || szName == NULL || puCompSize == NULL ||
	    puUncompSize == NULL || puMethod == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (cbLen < B123_CDFH_MIN) {
		errno = EINVAL;
		return -1;
	}

	pBase = (const unsigned char *)pCd;
	pEnd = pBase + cbLen;
	pCur = pBase;
	uCur = 0u;

	while (pCur + B123_CDFH_MIN <= pEnd) {
		uint32_t uSig;
		uint16_t u16Method;
		uint32_t u32Comp;
		uint32_t u32Uncomp;
		uint16_t u16Name;
		uint16_t u16Extra;
		uint16_t u16Cmt;
		size_t cbVar;
		size_t cbLeft;
		size_t cbCopy;
		size_t i;

		uSig = b123_rd32(pCur);
		if (uSig != B123_SIG_CDFH) {
			errno = EINVAL;
			return -1;
		}

		u16Method = b123_rd16(pCur + 10);
		u32Comp = b123_rd32(pCur + 20);
		u32Uncomp = b123_rd32(pCur + 24);
		u16Name = b123_rd16(pCur + 28);
		u16Extra = b123_rd16(pCur + 30);
		u16Cmt = b123_rd16(pCur + 32);

		cbVar = (size_t)u16Name + (size_t)u16Extra + (size_t)u16Cmt;
		cbLeft = (size_t)(pEnd - pCur);
		if (cbLeft < B123_CDFH_MIN ||
		    cbVar > cbLeft - B123_CDFH_MIN) {
			errno = EINVAL;
			return -1;
		}

		if (uCur == uIdx) {
			*puMethod = u16Method;
			*puCompSize = u32Comp;
			*puUncompSize = u32Uncomp;

			/* Copy name into fixed 256 buffer with trailing NUL. */
			cbCopy = (size_t)u16Name;
			if (cbCopy > 255u) {
				cbCopy = 255u;
			}
			if (cbCopy > 0u) {
				b123_copy(szName, pCur + B123_CDFH_MIN, cbCopy);
			}
			szName[cbCopy] = '\0';
			/* Scrub any embedded NULs for C-string consumers. */
			for (i = 0u; i < cbCopy; i++) {
				if (szName[i] == '\0') {
					szName[i] = '?';
				}
			}
			return 0;
		}

		pCur += B123_CDFH_MIN + cbVar;
		uCur++;
	}

	errno = ENOENT;
	return -1;
}
