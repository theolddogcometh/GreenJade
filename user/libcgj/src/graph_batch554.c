/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch554: length-bounded URL percent-decode.
 *
 * Surface (unique symbols — does NOT redefine gj_url_decode / gj_pct_decode):
 *   int gj_url_decode_n(const char *in, size_t n, unsigned char *out,
 *                       size_t cap);
 *     — Percent-decode the first n bytes of in into out (capacity cap).
 *       '%' followed by two hex digits (case-insensitive) becomes one
 *       octet; all other input octets copy through unchanged.
 *       Strict: '+' is NOT mapped to space (stays as '+').
 *       Incomplete or non-hex % sequences → -1.
 *       On success returns the number of decoded bytes written.
 *       Output is raw octets (no forced trailing NUL).
 *       Returns -1 on error (NULL in when n > 0, NULL out when any
 *       byte would be written, cap too small, invalid encoding, or
 *       decoded length not representable as int).
 *   __gj_url_decode_n  (alias)
 *   __libcgj_batch554_marker = "libcgj-batch554"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_url_decode     → graph_batch103.c  (NUL-terminated char out + errno)
 *   gj_pct_decode     → graph_batch274.c  (NUL-terminated, out_n pointer)
 *   gj_form_get       → graph_batch197.c  ('+' → space form body)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch554_marker[] = "libcgj-batch554";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b554_hex_val — map ASCII hex digit to 0..15, or -1 if not hex.
 */
static int
b554_hex_val(unsigned char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (int)(ch - '0');
	}
	if (ch >= 'A' && ch <= 'F') {
		return (int)(ch - 'A') + 10;
	}
	if (ch >= 'a' && ch <= 'f') {
		return (int)(ch - 'a') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_decode_n — strict percent-decode of an n-byte input slice.
 *
 * in:   source bytes (may contain embedded NULs; only first n are read).
 * n:    number of input bytes to consume.
 * out:  destination buffer for decoded octets.
 * cap:  capacity of out in bytes.
 *
 * Semantics:
 *   - Literal bytes other than '%' copy as-is (including '+').
 *   - "%HH" with H hex (A-F/a-f/0-9) → one byte (hi<<4)|lo.
 *   - Truncated "%" / "%H" or non-hex → -1.
 *   - Empty input (n == 0) succeeds with return 0 (out may be NULL).
 */
int
gj_url_decode_n(const char *in, size_t n, unsigned char *out, size_t cap)
{
	size_t iIn;
	size_t cbOut;
	unsigned char ch;
	int nHi;
	int nLo;

	if (n == 0u) {
		return 0;
	}
	if (in == NULL) {
		return -1;
	}

	iIn = 0u;
	cbOut = 0u;

	while (iIn < n) {
		ch = (unsigned char)in[iIn];
		if (ch == (unsigned char)'%') {
			if (iIn + 2u >= n) {
				return -1;
			}
			nHi = b554_hex_val((unsigned char)in[iIn + 1u]);
			nLo = b554_hex_val((unsigned char)in[iIn + 2u]);
			if (nHi < 0 || nLo < 0) {
				return -1;
			}
			if (cbOut >= cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (unsigned char)((nHi << 4) | nLo);
			cbOut++;
			iIn += 3u;
		} else {
			if (cbOut >= cap || out == NULL) {
				return -1;
			}
			out[cbOut] = ch;
			cbOut++;
			iIn++;
		}
	}

	if (cbOut > (size_t)INT32_MAX) {
		return -1;
	}
	return (int)cbOut;
}

int __gj_url_decode_n(const char *in, size_t n, unsigned char *out,
    size_t cap)
    __attribute__((alias("gj_url_decode_n")));
