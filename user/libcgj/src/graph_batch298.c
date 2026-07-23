/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch298: lowercase hex (base16) encode/decode.
 *
 * Surface (unique symbols — do NOT collide with hex_encode/hex_decode):
 *   int gj_hex_encode(const uint8_t *in, size_t nin, char *out,
 *                     size_t out_cap);
 *     — Encode nin bytes at in as lowercase hex into out (NUL-terminated).
 *       Each input byte becomes two ASCII hex digits ('0'-'9', 'a'-'f').
 *       out_cap includes the trailing NUL (need 2*nin + 1 on success).
 *       Returns 0 on success, -1 on error (NULL out, NULL in with nin > 0,
 *       out_cap too small, or 2*nin overflow).
 *       Empty input (nin == 0, in may be NULL) writes "" when out_cap >= 1.
 *   int gj_hex_decode(const char *in, uint8_t *out, size_t out_cap,
 *                     size_t *nout);
 *     — Decode a NUL-terminated hex string at in into binary out.
 *       Digits are case-insensitive ('0'-'9', 'a'-'f', 'A'-'F').
 *       Strict: only hex digits allowed (no whitespace, no separators);
 *       odd length is an error. On success *nout = decoded byte count.
 *       Returns 0 / -1. Empty input succeeds with *nout = 0.
 *   __gj_hex_encode / __gj_hex_decode  (aliases)
 *   __libcgj_batch298_marker = "libcgj-batch298"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   hex_encode / hex_decode → graph_batch41.c (ssize_t, errno, whitespace)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch298_marker[] = "libcgj-batch298";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b298_hex_digit — nibble 0..15 → lowercase hex ASCII ('0'-'9', 'a'-'f').
 */
static char
b298_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + (char)uNib)
	                           : ('a' + (char)(uNib - 10u)));
}

/*
 * b298_hex_val — map ASCII hex digit to 0..15, or -1 if not hex.
 */
static int
b298_hex_val(unsigned char ch)
{
	if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') {
		return (int)(ch - (unsigned char)'0');
	}
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'F') {
		return (int)(ch - (unsigned char)'A') + 10;
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'f') {
		return (int)(ch - (unsigned char)'a') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_encode — lowercase base16 of nin octets into a C string.
 *
 * in:      input bytes (NULL only legal when nin == 0)
 * nin:     input length in bytes
 * out:     destination C string buffer (must be non-NULL)
 * out_cap: capacity of out including the terminating NUL
 *
 * Edge cases:
 *   out == NULL                         → -1
 *   in == NULL && nin != 0              → -1
 *   out_cap == 0                        → -1
 *   nin == 0 (empty) with out_cap >= 1  → out = "", return 0
 *   2*nin + 1 > out_cap (or overflow)   → -1 (out undefined)
 */
int
gj_hex_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
{
	size_t i;
	size_t nNeed;

	if (out == NULL || out_cap == 0u) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}

	/*
	 * Need 2*nin + 1 bytes (digits + NUL). Guard against size_t
	 * overflow when nin is near SIZE_MAX / 2.
	 */
	if (nin > ((size_t)-1 - 1u) / 2u) {
		return -1;
	}
	nNeed = nin * 2u + 1u;
	if (out_cap < nNeed) {
		return -1;
	}

	for (i = 0u; i < nin; i++) {
		uint8_t uByte = in[i];

		out[i * 2u] = b298_hex_digit((unsigned)(uByte >> 4) & 0x0fu);
		out[i * 2u + 1u] = b298_hex_digit((unsigned)uByte & 0x0fu);
	}
	out[nin * 2u] = '\0';
	return 0;
}

/*
 * gj_hex_decode — strict base16 decode of a NUL-terminated hex string.
 *
 * in:      NUL-terminated hex input (required)
 * out:     destination buffer for decoded octets (may be NULL only when
 *          the decoded length is 0)
 * out_cap: capacity of out in bytes
 * nout:    on success, set to the number of decoded bytes written
 *
 * Semantics:
 *   - Only hex digits (case-insensitive) are accepted; no whitespace.
 *   - Length (number of digits) must be even.
 *   - Empty input succeeds with *nout == 0 (out may be NULL if out_cap
 *     is 0 or unused).
 *   - On error *nout is undefined.
 */
int
gj_hex_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
{
	size_t iIn;
	size_t nDigits;
	size_t nBytes;
	size_t i;

	if (in == NULL || nout == NULL) {
		return -1;
	}

	/* Count digits and reject non-hex early. */
	nDigits = 0u;
	for (iIn = 0u; in[iIn] != '\0'; iIn++) {
		if (b298_hex_val((unsigned char)in[iIn]) < 0) {
			return -1;
		}
		nDigits++;
	}

	if ((nDigits & 1u) != 0u) {
		return -1;
	}

	nBytes = nDigits / 2u;
	if (nBytes > out_cap) {
		return -1;
	}
	if (nBytes > 0u && out == NULL) {
		return -1;
	}

	for (i = 0u; i < nBytes; i++) {
		int nHi = b298_hex_val((unsigned char)in[i * 2u]);
		int nLo = b298_hex_val((unsigned char)in[i * 2u + 1u]);

		/* Already validated as hex above; keep defensive. */
		if (nHi < 0 || nLo < 0) {
			return -1;
		}
		out[i] = (uint8_t)(((unsigned)nHi << 4) | (unsigned)nLo);
	}

	*nout = nBytes;
	return 0;
}

int __gj_hex_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
    __attribute__((alias("gj_hex_encode")));
int __gj_hex_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
    __attribute__((alias("gj_hex_decode")));
