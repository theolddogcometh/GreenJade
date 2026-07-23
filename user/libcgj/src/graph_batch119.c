/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch119: Crockford Base32 encode / decode.
 * Clean-room freestanding pure C (integer/pointer; builds with -msse2).
 * No third-party Crockford source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base32_encode / base32_decode           → graph_batch41.c (RFC 4648)
 *   hex_encode / hex_decode                 → graph_batch41.c (base16)
 *   base32hex_encode / base32hex_decode     → graph_batch42.c (RFC 4648 §7)
 *   base85 / ascii85 / base58 / base64…     → earlier graph batches
 *
 * This TU adds only unique symbols:
 *   ssize_t crockford32_encode(const unsigned char *in, size_t inlen,
 *                              char *out, size_t outcap);
 *   ssize_t crockford32_decode(const char *in, size_t inlen,
 *                              unsigned char *out, size_t outcap);
 *   __crockford32_encode / __crockford32_decode  (aliases)
 *   __libcgj_batch119_marker = "libcgj-batch119"
 *
 * Crockford alphabet (encode, uppercase):
 *   0123456789ABCDEFGHJKMNPQRSTVWXYZ
 *   (I, L, O, U excluded from the encode set)
 *
 * Decode accepts case-insensitive symbols and Crockford confusables:
 *   O/o → 0,  I/i/L/l → 1; hyphens are ignored as group separators.
 *   No '=' padding (unlike RFC 4648 base32). U is not a data symbol.
 *
 * Encode API (matches base32/base85 graph shape):
 *   out == NULL  → returns bytes needed including trailing NUL
 *   success      → returns encoded length excluding NUL
 *   error        → -1 (EINVAL / ENOSPC)
 *
 * Decode: skips ASCII whitespace and '-'; returns decoded byte count,
 * or -1 with errno. out == NULL counts without writing.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch119_marker[] = "libcgj-batch119";

/* Crockford encode alphabet (32 symbols; no I/L/O/U). */
static const char s_b119_alpha[32] =
    "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

/*
 * Decode map for ASCII 0..127: -1 invalid, else 0..31.
 * Built at first use so the table stays compact and reviewable.
 */
static signed char s_b119_dec[128];
static int s_b119_dec_ready;

static void
b119_init_dec(void)
{
	unsigned i;

	if (s_b119_dec_ready) {
		return;
	}
	for (i = 0; i < 128u; i++) {
		s_b119_dec[i] = (signed char)-1;
	}
	/* Digits 0-9 */
	for (i = 0; i < 10u; i++) {
		s_b119_dec['0' + i] = (signed char)i;
	}
	/* A-Z / a-z excluding I,L,O,U from encode set — map rest. */
	/* A=10 B=11 C=12 D=13 E=14 F=15 G=16 H=17 */
	s_b119_dec['A'] = 10;
	s_b119_dec['B'] = 11;
	s_b119_dec['C'] = 12;
	s_b119_dec['D'] = 13;
	s_b119_dec['E'] = 14;
	s_b119_dec['F'] = 15;
	s_b119_dec['G'] = 16;
	s_b119_dec['H'] = 17;
	/* I → 1 (confusable) */
	s_b119_dec['I'] = 1;
	/* J=18 K=19 */
	s_b119_dec['J'] = 18;
	s_b119_dec['K'] = 19;
	/* L → 1 (confusable) */
	s_b119_dec['L'] = 1;
	/* M=20 N=21 */
	s_b119_dec['M'] = 20;
	s_b119_dec['N'] = 21;
	/* O → 0 (confusable) */
	s_b119_dec['O'] = 0;
	/* P=22 Q=23 R=24 S=25 T=26 */
	s_b119_dec['P'] = 22;
	s_b119_dec['Q'] = 23;
	s_b119_dec['R'] = 24;
	s_b119_dec['S'] = 25;
	s_b119_dec['T'] = 26;
	/* U not a data symbol (checksum-only in Crockford scheme) */
	/* V=27 W=28 X=29 Y=30 Z=31 */
	s_b119_dec['V'] = 27;
	s_b119_dec['W'] = 28;
	s_b119_dec['X'] = 29;
	s_b119_dec['Y'] = 30;
	s_b119_dec['Z'] = 31;

	/* Lowercase mirrors (and confusables). */
	s_b119_dec['a'] = 10;
	s_b119_dec['b'] = 11;
	s_b119_dec['c'] = 12;
	s_b119_dec['d'] = 13;
	s_b119_dec['e'] = 14;
	s_b119_dec['f'] = 15;
	s_b119_dec['g'] = 16;
	s_b119_dec['h'] = 17;
	s_b119_dec['i'] = 1;
	s_b119_dec['j'] = 18;
	s_b119_dec['k'] = 19;
	s_b119_dec['l'] = 1;
	s_b119_dec['m'] = 20;
	s_b119_dec['n'] = 21;
	s_b119_dec['o'] = 0;
	s_b119_dec['p'] = 22;
	s_b119_dec['q'] = 23;
	s_b119_dec['r'] = 24;
	s_b119_dec['s'] = 25;
	s_b119_dec['t'] = 26;
	s_b119_dec['v'] = 27;
	s_b119_dec['w'] = 28;
	s_b119_dec['x'] = 29;
	s_b119_dec['y'] = 30;
	s_b119_dec['z'] = 31;

	s_b119_dec_ready = 1;
}

static int
b119_is_ws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
	       ch == '\f' || ch == '\v';
}

/*
 * Encoded symbol count for cbIn input bytes (no padding, no NUL):
 *   ceil(cbIn * 8 / 5) = (cbIn * 8 + 4) / 5
 */
static size_t
b119_enc_len(size_t cbIn)
{
	if (cbIn > (SIZE_MAX - 4u) / 8u) {
		return SIZE_MAX;
	}
	return (cbIn * 8u + 4u) / 5u;
}

/*
 * crockford32_encode — stream 8-bit → 5-bit Crockford symbols.
 * Remainder bits are zero-padded on the low side (no '=').
 */
ssize_t
crockford32_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
                   size_t cbOut)
{
	size_t cbSym;
	size_t cbNeed;
	size_t iIn;
	size_t iOut;
	unsigned uBuf;
	int nBits;

	if (pIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}

	cbSym = b119_enc_len(cbIn);
	if (cbSym == SIZE_MAX || cbSym + 1u < cbSym) {
		errno = EOVERFLOW;
		return -1;
	}
	/* +1 for trailing NUL */
	cbNeed = cbSym + 1u;

	if (szOut == NULL) {
		return (ssize_t)cbNeed;
	}
	if (cbOut < cbNeed) {
		errno = ENOSPC;
		return -1;
	}

	iIn = 0u;
	iOut = 0u;
	uBuf = 0u;
	nBits = 0;

	while (iIn < cbIn) {
		uBuf = (uBuf << 8) | (unsigned)pIn[iIn++];
		nBits += 8;
		while (nBits >= 5) {
			unsigned uSym;

			nBits -= 5;
			uSym = (uBuf >> (unsigned)nBits) & 0x1fu;
			szOut[iOut++] = s_b119_alpha[uSym];
		}
	}
	if (nBits > 0) {
		unsigned uSym;

		uSym = (uBuf << (unsigned)(5 - nBits)) & 0x1fu;
		szOut[iOut++] = s_b119_alpha[uSym];
	}

	szOut[iOut] = '\0';
	return (ssize_t)iOut;
}

/*
 * crockford32_decode — stream Crockford symbols → bytes.
 * Ignores ASCII whitespace and '-'. Rejects other non-alphabet input
 * (including 'U'/'u' and '=').
 */
ssize_t
crockford32_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
                   size_t cbOut)
{
	size_t iIn;
	size_t iOut;
	unsigned uBuf;
	int nBits;

	if (szIn == NULL && cbIn != 0u) {
		errno = EINVAL;
		return -1;
	}

	b119_init_dec();

	iIn = 0u;
	iOut = 0u;
	uBuf = 0u;
	nBits = 0;

	while (iIn < cbIn) {
		unsigned char ch;
		int nVal;

		ch = (unsigned char)szIn[iIn++];
		if (ch == (unsigned char)'-' || b119_is_ws(ch)) {
			continue;
		}
		if (ch >= 128u) {
			errno = EINVAL;
			return -1;
		}
		nVal = (int)s_b119_dec[ch];
		if (nVal < 0) {
			errno = EINVAL;
			return -1;
		}

		uBuf = (uBuf << 5) | (unsigned)nVal;
		nBits += 5;
		if (nBits >= 8) {
			nBits -= 8;
			if (pOut != NULL) {
				if (iOut >= cbOut) {
					errno = ENOSPC;
					return -1;
				}
				pOut[iOut] = (unsigned char)((uBuf >>
				    (unsigned)nBits) & 0xffu);
			}
			iOut++;
		}
	}

	/* Trailing partial symbol bits (zero pad residue) are discarded. */
	return (ssize_t)iOut;
}

ssize_t __crockford32_encode(const unsigned char *p, size_t n, char *s,
                             size_t m)
    __attribute__((alias("crockford32_encode")));
ssize_t __crockford32_decode(const char *s, size_t n, unsigned char *p,
                             size_t m)
    __attribute__((alias("crockford32_decode")));
