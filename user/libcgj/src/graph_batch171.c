/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch171: Base36 encode / decode (byte-string).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party base36 source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   base32hex_encode / base32hex_decode / __base32hex_*
 *     → graph_batch42.c  (RFC 4648 §7 base32hex, bit-pack + pad)
 *   base32_encode / base32_decode → graph_batch41.c
 *   base58_encode / base58_decode → graph_batch42.c
 *   gj_base58check_encode         → graph_batch164.c
 *
 * This TU adds only unique symbols (base32hex exists uniquely → base36):
 *   int gj_base36_encode(const unsigned char *in, size_t n,
 *                        char *out, size_t cap);
 *     — multiprecision base-36 encode of a big-endian byte string.
 *       Alphabet: "0123456789abcdefghijklmnopqrstuvwxyz".
 *       Leading 0x00 input bytes become leading '0' output digits.
 *       Writes a NUL-terminated string when cap allows.
 *       Returns encoded length (NUL not counted) on success, -1 on error.
 *   int gj_base36_decode(const char *in, size_t n, unsigned char *out,
 *                        size_t cap, size_t *olen);
 *     — inverse: base-36 text → big-endian bytes. Accepts A-Z as a-z.
 *       Leading '0' digits become leading 0x00 bytes.
 *       *olen = decoded byte count on success. Returns 0 / -1.
 *   __gj_base36_encode / __gj_base36_decode  (aliases)
 *   __libcgj_batch171_marker = "libcgj-batch171"
 *
 * Notes:
 *   - Stack workspace caps input ~330 bytes (encode) / ~500 chars (decode).
 *   - Empty input encodes to empty string (length 0) / decodes to olen 0.
 *   - No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch171_marker[] = "libcgj-batch171";

/* Workspace: log(256)/log(36) ≈ 1.547 → size = n*155/100 + 1 ≤ 512. */
#define B171_WORK_MAX  512u
#define B171_MAX_IN    330u

/* Base36 alphabet (lowercase). */
static const char s_b171_alpha[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b171_memset(void *pDst, int nByte, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = (unsigned char)nByte;
	}
}

/*
 * Map one base36 digit to value 0..35. Accepts 0-9, a-z, A-Z.
 * Returns -1 on invalid.
 */
static int
b171_val(int ch)
{
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	}
	if (ch >= 'a' && ch <= 'z') {
		return ch - 'a' + 10;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return ch - 'A' + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base36_encode — big-endian byte string → base36 text.
 *
 * in may be NULL only when n == 0. Leading zero bytes map to '0'.
 * Returns length excluding trailing NUL, or -1 on error.
 */
int
gj_base36_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                 size_t cbCap)
{
	unsigned char aBuf[B171_WORK_MAX];
	size_t cZeros = 0u;
	size_t i;
	size_t cbSize;
	size_t cbHigh;
	size_t cbLen;
	char *pDst;

	if (pIn == NULL && cbIn != 0u) {
		return -1;
	}
	if (szOut == NULL || cbCap == 0u) {
		return -1;
	}
	if (cbIn > B171_MAX_IN) {
		return -1;
	}

	while (cZeros < cbIn && pIn[cZeros] == 0u) {
		cZeros++;
	}

	/* log(256)/log(36) ≈ 1.547 → size = cbIn*155/100 + 1 */
	cbSize = cbIn * 155u / 100u + 1u;
	if (cbSize > sizeof(aBuf)) {
		return -1;
	}
	b171_memset(aBuf, 0, cbSize);

	for (i = cZeros; i < cbIn; i++) {
		unsigned uCarry = (unsigned)pIn[i];
		size_t j;

		for (j = cbSize; j > 0u; j--) {
			uCarry += 256u * (unsigned)aBuf[j - 1u];
			aBuf[j - 1u] = (unsigned char)(uCarry % 36u);
			uCarry /= 36u;
		}
	}

	cbHigh = 0u;
	while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
		cbHigh++;
	}

	/*
	 * Empty input → empty string. Non-empty all-zero → cZeros '0' digits.
	 * Non-zero residual digits are taken from aBuf[cbHigh..).
	 */
	cbLen = cZeros + (cbSize - cbHigh);
	if (cbLen + 1u > cbCap) {
		return -1;
	}

	pDst = szOut;
	for (i = 0u; i < cZeros; i++) {
		*pDst++ = '0';
	}
	for (i = cbHigh; i < cbSize; i++) {
		*pDst++ = s_b171_alpha[aBuf[i]];
	}
	*pDst = '\0';
	return (int)(pDst - szOut);
}

int __gj_base36_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                       size_t cbCap)
    __attribute__((alias("gj_base36_encode")));

/*
 * gj_base36_decode — base36 text → big-endian byte string.
 *
 * in may be NULL only when n == 0. Leading '0' digits become 0x00 bytes.
 * *olen receives decoded length on success. Returns 0 ok, -1 on error.
 */
int
gj_base36_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
                 size_t cbCap, size_t *pcbOlen)
{
	unsigned char aBuf[B171_WORK_MAX];
	size_t cZeros = 0u;
	size_t i;
	size_t cbSize;
	size_t cbHigh;
	size_t cbLen;

	if (pcbOlen == NULL) {
		return -1;
	}
	if (szIn == NULL && cbIn != 0u) {
		return -1;
	}
	if (pOut == NULL && cbCap != 0u) {
		return -1;
	}

	if (cbIn == 0u) {
		*pcbOlen = 0u;
		return 0;
	}

	while (cZeros < cbIn && szIn[cZeros] == '0') {
		cZeros++;
	}

	/* log(36)/log(256) ≈ 0.646 → size = cbIn*647/1000 + 1 */
	cbSize = cbIn * 647u / 1000u + 1u;
	if (cbSize > sizeof(aBuf)) {
		return -1;
	}
	b171_memset(aBuf, 0, cbSize);

	for (i = cZeros; i < cbIn; i++) {
		int nVal = b171_val((unsigned char)szIn[i]);
		unsigned uCarry;
		size_t j;

		if (nVal < 0) {
			return -1;
		}
		uCarry = (unsigned)nVal;
		for (j = cbSize; j > 0u; j--) {
			uCarry += 36u * (unsigned)aBuf[j - 1u];
			aBuf[j - 1u] = (unsigned char)(uCarry & 0xffu);
			uCarry >>= 8;
		}
		if (uCarry != 0u) {
			return -1;
		}
	}

	cbHigh = 0u;
	while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
		cbHigh++;
	}

	cbLen = cZeros + (cbSize - cbHigh);
	if (cbLen > cbCap) {
		return -1;
	}
	if (pOut == NULL && cbLen > 0u) {
		return -1;
	}

	for (i = 0u; i < cZeros; i++) {
		pOut[i] = 0u;
	}
	for (i = cbHigh; i < cbSize; i++) {
		pOut[cZeros + (i - cbHigh)] = aBuf[i];
	}
	*pcbOlen = cbLen;
	return 0;
}

int __gj_base36_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
                       size_t cbCap, size_t *pcbOlen)
    __attribute__((alias("gj_base36_decode")));
