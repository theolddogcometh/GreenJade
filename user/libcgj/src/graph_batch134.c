/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch134: useful crypto helpers — buffer XOR,
 * keystream XOR, and PKCS #7 pad/unpad (RFC 5652 / PKCS #7 §10.3).
 * Clean-room freestanding pure C — builds under userspace -msse2.
 * No third-party crypto source was copied.
 *
 * This TU adds only unique symbols:
 *   void gj_xor_buf(unsigned char *dst, const unsigned char *a,
 *                   const unsigned char *b, size_t n);
 *     — dst[i] = a[i] ^ b[i] for i in [0, n). No-op on bad args
 *       (NULL dst/a/b with n > 0). SSE2 bulk path when n >= 16.
 *   void gj_xor_keystream(unsigned char *buf, size_t n,
 *                         const unsigned char *key, size_t klen);
 *     — in-place buf[i] ^= key[i % klen]. No-op if buf/key NULL with
 *       n/klen > 0, or if n == 0 or klen == 0.
 *   int gj_pkcs7_pad(unsigned char *buf, size_t len, size_t block,
 *                    size_t *outlen, size_t cap);
 *     — append PKCS #7 padding into buf[len..] up to cap. Always adds
 *       1..block pad bytes (full block when len % block == 0).
 *       *outlen = padded length. Returns 0 ok, -1 error.
 *   int gj_pkcs7_unpad(const unsigned char *buf, size_t len, size_t block,
 *                      size_t *outlen);
 *     — validate and strip PKCS #7 padding. Pad length = last byte in
 *       [1, block]; all pad bytes must equal that value. *outlen =
 *       unpadded length. Returns 0 ok, -1 error.
 *   __gj_xor_buf / __gj_xor_keystream / __gj_pkcs7_pad / __gj_pkcs7_unpad
 *   __libcgj_batch134_marker = "libcgj-batch134"
 *
 * block must be in [1, 255] for pad/unpad (PKCS #7 single-byte pad count).
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__)
#include <emmintrin.h>
#endif

const char __libcgj_batch134_marker[] = "libcgj-batch134";

/* ---- gj_xor_buf -------------------------------------------------------- */

/*
 * Bytewise XOR of two equal-length buffers into dst. When a or b aliases
 * dst, the sequential write is well-defined (same as in-place XOR).
 */
void
gj_xor_buf(unsigned char *pDst, const unsigned char *pA,
           const unsigned char *pB, size_t cb)
{
	size_t i = 0u;

	if (pDst == NULL || pA == NULL || pB == NULL || cb == 0u) {
		return;
	}

#if defined(__x86_64__) || defined(__i386__)
	if (cb >= 16u) {
		size_t nBlk = cb & ~(size_t)15u;

		while (i < nBlk) {
			__m128i vA = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pA + i));
			__m128i vB = _mm_loadu_si128(
			    (const __m128i *)(const void *)(pB + i));
			__m128i vX = _mm_xor_si128(vA, vB);

			_mm_storeu_si128((__m128i *)(void *)(pDst + i), vX);
			i += 16u;
		}
	}
#endif
	while (i < cb) {
		pDst[i] = (unsigned char)(pA[i] ^ pB[i]);
		i++;
	}
}

void __gj_xor_buf(unsigned char *dst, const unsigned char *a,
                  const unsigned char *b, size_t n)
    __attribute__((alias("gj_xor_buf")));

/* ---- gj_xor_keystream -------------------------------------------------- */

/*
 * In-place XOR of buf with a repeating keystream key[0..klen).
 * Common pattern for stream-cipher style post-processing and simple
 * OTP-style mixing when the caller supplies a full-length key.
 */
void
gj_xor_keystream(unsigned char *pBuf, size_t cb,
                 const unsigned char *pKey, size_t cbKey)
{
	size_t i;
	size_t iKey;

	if (pBuf == NULL || pKey == NULL || cb == 0u || cbKey == 0u) {
		return;
	}

	iKey = 0u;
	for (i = 0u; i < cb; i++) {
		pBuf[i] = (unsigned char)(pBuf[i] ^ pKey[iKey]);
		iKey++;
		if (iKey >= cbKey) {
			iKey = 0u;
		}
	}
}

void __gj_xor_keystream(unsigned char *buf, size_t n,
                        const unsigned char *key, size_t klen)
    __attribute__((alias("gj_xor_keystream")));

/* ---- gj_pkcs7_pad ------------------------------------------------------ */

/*
 * PKCS #7 padding (also used by PKCS #5 for 8-byte blocks). Pad count
 * pad = block - (len % block), with pad == block when len is already a
 * multiple of block. Each of the pad trailing bytes is set to (unsigned
 * char)pad. Requires room in [len, cap).
 */
int
gj_pkcs7_pad(unsigned char *pBuf, size_t cbLen, size_t cbBlock,
             size_t *pcbOut, size_t cbCap)
{
	size_t cbPad;
	size_t cbFinal;
	size_t i;
	unsigned char chPad;

	if (pBuf == NULL || pcbOut == NULL) {
		return -1;
	}
	/* PKCS #7 pad byte is a single octet → block in [1, 255]. */
	if (cbBlock == 0u || cbBlock > 255u) {
		return -1;
	}
	if (cbLen > cbCap) {
		return -1;
	}

	cbPad = cbBlock - (cbLen % cbBlock);
	/* Overflow-safe: need cbLen + cbPad <= cbCap. */
	if (cbPad > cbCap - cbLen) {
		return -1;
	}

	cbFinal = cbLen + cbPad;
	chPad = (unsigned char)cbPad;
	for (i = 0u; i < cbPad; i++) {
		pBuf[cbLen + i] = chPad;
	}
	*pcbOut = cbFinal;
	return 0;
}

int __gj_pkcs7_pad(unsigned char *buf, size_t len, size_t block,
                   size_t *outlen, size_t cap)
    __attribute__((alias("gj_pkcs7_pad")));

/* ---- gj_pkcs7_unpad ---------------------------------------------------- */

/*
 * Strip and validate PKCS #7 padding. Rejects truncated buffers, zero
 * pad length, pad length > block, pad length > len, and any pad byte
 * that does not match the declared pad length.
 */
int
gj_pkcs7_unpad(const unsigned char *pBuf, size_t cbLen, size_t cbBlock,
               size_t *pcbOut)
{
	size_t cbPad;
	size_t i;
	unsigned char chPad;

	if (pBuf == NULL || pcbOut == NULL) {
		return -1;
	}
	if (cbBlock == 0u || cbBlock > 255u) {
		return -1;
	}
	if (cbLen == 0u || (cbLen % cbBlock) != 0u) {
		return -1;
	}

	chPad = pBuf[cbLen - 1u];
	cbPad = (size_t)chPad;
	if (cbPad == 0u || cbPad > cbBlock || cbPad > cbLen) {
		return -1;
	}

	for (i = 0u; i < cbPad; i++) {
		if (pBuf[cbLen - 1u - i] != chPad) {
			return -1;
		}
	}

	*pcbOut = cbLen - cbPad;
	return 0;
}

int __gj_pkcs7_unpad(const unsigned char *buf, size_t len, size_t block,
                     size_t *outlen)
    __attribute__((alias("gj_pkcs7_unpad")));
