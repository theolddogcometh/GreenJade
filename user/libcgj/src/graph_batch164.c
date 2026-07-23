/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch164: Bitcoin-style Base58Check encode.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party base58/Bitcoin source
 * was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   base58_encode / base58_decode / __base58_encode / __base58_decode
 *     → graph_batch42.c  (raw Base58, no checksum)
 *   SHA256_Init / SHA256_Update / SHA256_Final
 *     → graph_batch31/32 (libmd digest) — used as extern for double-SHA256
 *
 * This TU adds only unique symbols:
 *   int gj_base58check_encode(const unsigned char *payload, size_t plen,
 *                             char *out, size_t cap);
 *     — Base58Check: append first 4 bytes of SHA256d(payload) as checksum,
 *       then Bitcoin-alphabet Base58 encode (leading 0x00 → '1').
 *       Writes a NUL-terminated string into out when cap allows.
 *       Returns encoded length (NUL not counted) on success, -1 on error
 *       (NULL args / empty cap / payload too large / output too small).
 *   __gj_base58check_encode  (alias)
 *   __libcgj_batch164_marker = "libcgj-batch164"
 *
 * Double-SHA256 checksum uses extern SHA256_* when available at link
 * time (undefined references are fine for a freestanding -c unit).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch164_marker[] = "libcgj-batch164";

/* Extern SHA256 (libmd layout; ctx size >= 112 bytes in batch31). */
void SHA256_Init(void *pCtx);
void SHA256_Update(void *pCtx, const void *pData, size_t cb);
void SHA256_Final(unsigned char aDig[32], void *pCtx);

/* Max payload: room for +4 checksum in encode workspace (~512 bytes). */
#define B164_MAX_PAYLOAD  360u
#define B164_CHK          4u
#define B164_WORK_MAX     512u
#define B164_SHA_CTX      128u

/* Bitcoin Base58 alphabet (no 0 O I l). */
static const char s_b164_b58_alpha[] =
	"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b164_memset(void *pDst, int nByte, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = (unsigned char)nByte;
	}
}

static void
b164_memcpy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD = (unsigned char *)pDst;
	const unsigned char *pS = (const unsigned char *)pSrc;
	size_t i;

	for (i = 0u; i < cb; i++) {
		pD[i] = pS[i];
	}
}

/*
 * SHA256d: first 32-byte digest of SHA256(SHA256(pData)).
 * Uses extern SHA256_Init / Update / Final (ctx layout matches batch31).
 */
static void
b164_sha256d(const unsigned char *pData, size_t cb,
             unsigned char aOut[32])
{
	unsigned char aCtx[B164_SHA_CTX];
	unsigned char aMid[32];

	b164_memset(aCtx, 0, sizeof(aCtx));
	SHA256_Init(aCtx);
	if (cb > 0u && pData != NULL) {
		SHA256_Update(aCtx, pData, cb);
	}
	SHA256_Final(aMid, aCtx);

	b164_memset(aCtx, 0, sizeof(aCtx));
	SHA256_Init(aCtx);
	SHA256_Update(aCtx, aMid, 32u);
	SHA256_Final(aOut, aCtx);
}

/*
 * Raw Bitcoin Base58 encode of pIn[0..cbIn).
 * Writes NUL-terminated string; returns length (no NUL) or -1.
 */
static int
b164_base58_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                   size_t cbOut)
{
	unsigned char aBuf[B164_WORK_MAX];
	size_t cZeros = 0u;
	size_t i;
	size_t cbSize;
	size_t cbHigh;
	size_t cbLen;
	char *pDst;

	if (pIn == NULL && cbIn != 0u) {
		return -1;
	}
	if (szOut == NULL || cbOut == 0u) {
		return -1;
	}

	while (cZeros < cbIn && pIn[cZeros] == 0u) {
		cZeros++;
	}

	/* log(256)/log(58) ≈ 1.37 → size = cbIn*138/100 + 1 */
	cbSize = cbIn * 138u / 100u + 1u;
	if (cbSize > sizeof(aBuf)) {
		return -1;
	}
	b164_memset(aBuf, 0, cbSize);

	for (i = cZeros; i < cbIn; i++) {
		unsigned uCarry = (unsigned)pIn[i];
		size_t j;

		for (j = cbSize; j > 0u; j--) {
			uCarry += 256u * (unsigned)aBuf[j - 1u];
			aBuf[j - 1u] = (unsigned char)(uCarry % 58u);
			uCarry /= 58u;
		}
	}

	cbHigh = 0u;
	while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
		cbHigh++;
	}

	cbLen = cZeros + (cbSize - cbHigh);
	if (cbLen + 1u > cbOut) {
		return -1;
	}

	pDst = szOut;
	for (i = 0u; i < cZeros; i++) {
		*pDst++ = '1';
	}
	for (i = cbHigh; i < cbSize; i++) {
		*pDst++ = s_b164_b58_alpha[aBuf[i]];
	}
	*pDst = '\0';
	return (int)(pDst - szOut);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base58check_encode — payload || SHA256d(payload)[0..3] → Base58.
 *
 * payload may be NULL only when plen == 0 (empty versioned payload).
 * Returns encoded length excluding trailing NUL, or -1 on error.
 */
int
gj_base58check_encode(const unsigned char *pPayload, size_t cbPayload,
                      char *szOut, size_t cbCap)
{
	unsigned char aExt[B164_MAX_PAYLOAD + B164_CHK];
	unsigned char aHash[32];

	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (pPayload == NULL && cbPayload != 0u) {
		return -1;
	}
	if (cbPayload > B164_MAX_PAYLOAD) {
		return -1;
	}

	if (cbPayload > 0u) {
		b164_memcpy(aExt, pPayload, cbPayload);
	}

	/* Double-SHA256 checksum (extern SHA256_*). */
	b164_sha256d(pPayload, cbPayload, aHash);
	aExt[cbPayload + 0u] = aHash[0];
	aExt[cbPayload + 1u] = aHash[1];
	aExt[cbPayload + 2u] = aHash[2];
	aExt[cbPayload + 3u] = aHash[3];

	return b164_base58_encode(aExt, cbPayload + B164_CHK, szOut, cbCap);
}

int __gj_base58check_encode(const unsigned char *pPayload, size_t cbPayload,
                            char *szOut, size_t cbCap)
    __attribute__((alias("gj_base58check_encode")));
