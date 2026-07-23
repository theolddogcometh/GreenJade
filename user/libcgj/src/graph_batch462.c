/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch462: freestanding Bitcoin Base58 decode.
 *
 * Surface (unique symbols — does NOT redefine base58_decode / __base58_decode
 * or gj_base58check_encode):
 *   int gj_b58_decode(const char *in, uint8_t *out, size_t out_cap,
 *                     size_t *nout);
 *     — Decode a NUL-terminated Bitcoin-alphabet Base58 string into binary
 *       out[0..out_cap). Leading '1' characters map to leading 0x00 bytes
 *       (Bitcoin convention). On success *nout = decoded byte count and
 *       return is 0. Empty input succeeds with *nout = 0.
 *       Returns -1 on error (NULL in/nout, NULL out when bytes needed,
 *       out_cap too small, invalid alphabet character, or input too large
 *       for the fixed workspace). *nout is undefined on error.
 *   int __gj_b58_decode  (alias)
 *   __libcgj_batch462_marker = "libcgj-batch462"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base58_encode / base58_decode / __base58_* → graph_batch42.c
 *   gj_base58check_encode                      → graph_batch164.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch462_marker[] = "libcgj-batch462";

/* Fixed workspace for big-integer digit conversion (stack only). */
#define B462_WORK_MAX  512u

/* Bitcoin Base58 alphabet (no 0 O I l). */
static const char s_b462_b58_alpha[] =
	"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b462_memset(void *pDst, int nByte, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = (unsigned char)nByte;
	}
}

/*
 * b462_b58_val — map one Bitcoin Base58 digit to 0..57, or -1 if invalid.
 * Strict alphabet only (case-sensitive; no whitespace).
 */
static int
b462_b58_val(unsigned char ch)
{
	const char *p = s_b462_b58_alpha;
	int i;

	for (i = 0; p[i] != '\0'; i++) {
		if ((unsigned char)p[i] == ch) {
			return i;
		}
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b58_decode — Bitcoin Base58 decode of a NUL-terminated C string.
 *
 * in:      NUL-terminated Base58 text (required).
 * out:     destination buffer (required whenever decoded length > 0;
 *          may be NULL only for empty successful decode).
 * out_cap: capacity of out in bytes.
 * nout:    on success, set to the number of decoded bytes written (required).
 *
 * Semantics:
 *   - Alphabet: 123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz
 *   - Leading '1' characters become leading 0x00 output octets.
 *   - No whitespace or separators; full-string decode to NUL.
 *   - Empty string → success, *nout = 0.
 *   - Input length is bounded by the fixed stack workspace (~512 result
 *     digits); oversized inputs fail with -1.
 *
 * Returns 0 on success, -1 on error.
 */
int
gj_b58_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
{
	unsigned char aBuf[B462_WORK_MAX];
	size_t cbIn;
	size_t cZeros;
	size_t i;
	size_t cbSize;
	size_t cbHigh;
	size_t cbLen;
	size_t iIn;

	if (in == NULL || nout == NULL) {
		return -1;
	}

	/* Measure input length (no libc strlen). */
	cbIn = 0u;
	while (in[cbIn] != '\0') {
		cbIn++;
	}

	if (cbIn == 0u) {
		*nout = 0u;
		return 0;
	}

	/* Leading '1' → leading zero bytes in the binary payload. */
	cZeros = 0u;
	while (cZeros < cbIn && in[cZeros] == '1') {
		cZeros++;
	}

	/*
	 * log(58)/log(256) ≈ 0.732 → size = cbIn * 733/1000 + 1.
	 * Bound against the fixed workspace.
	 */
	cbSize = cbIn * 733u / 1000u + 1u;
	if (cbSize > B462_WORK_MAX) {
		return -1;
	}
	b462_memset(aBuf, 0, cbSize);

	for (iIn = cZeros; iIn < cbIn; iIn++) {
		int nVal = b462_b58_val((unsigned char)in[iIn]);
		unsigned uCarry;
		size_t j;

		if (nVal < 0) {
			return -1;
		}
		uCarry = (unsigned)nVal;
		for (j = cbSize; j > 0u; j--) {
			uCarry += 58u * (unsigned)aBuf[j - 1u];
			aBuf[j - 1u] = (unsigned char)(uCarry & 0xffu);
			uCarry >>= 8;
		}
		if (uCarry != 0u) {
			/* Overflow beyond workspace — treat as hard failure. */
			return -1;
		}
	}

	/* Strip leading zero digits of the big-endian intermediate. */
	cbHigh = 0u;
	while (cbHigh < cbSize && aBuf[cbHigh] == 0u) {
		cbHigh++;
	}
	cbLen = cZeros + (cbSize - cbHigh);

	if (cbLen > out_cap) {
		return -1;
	}
	if (cbLen > 0u && out == NULL) {
		return -1;
	}

	/* Leading zeros from Base58 '1' prefix. */
	for (i = 0u; i < cZeros; i++) {
		out[i] = 0u;
	}
	/* Significant payload bytes (big-endian). */
	for (i = 0u; i < (cbSize - cbHigh); i++) {
		out[cZeros + i] = aBuf[cbHigh + i];
	}

	*nout = cbLen;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_b58_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
    __attribute__((alias("gj_b58_decode")));
