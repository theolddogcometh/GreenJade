/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch274: strict percent-decode (%XX only).
 *
 * Surface (unique symbols — does NOT redefine gj_url_decode):
 *   int gj_pct_decode(const char *in, unsigned char *out,
 *                     size_t out_cap, size_t *out_n);
 *     — Percent-decode NUL-terminated in into binary out.
 *       '%' followed by two hex digits (case-insensitive) becomes one
 *       octet; all other input octets copy through unchanged.
 *       Strict: '+' is NOT mapped to space (use form-urlencoded helpers
 *       for that). Incomplete or non-hex % sequences → -1.
 *       On success writes *out_n = decoded byte count and returns 0.
 *       Output is raw octets (no forced trailing NUL).
 *       Returns -1 on error (NULL in/out_n, NULL out when bytes needed,
 *       out_cap too small, or invalid encoding). *out_n undefined on
 *       error.
 *   __gj_pct_decode  (alias)
 *   __libcgj_batch274_marker = "libcgj-batch274"
 *
 * Companion encode (NOT redefined here):
 *   gj_pct_encode     → graph_batch273.c  (binary in → %HH text out)
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_url_decode     → graph_batch103.c  (NUL-terminated char out + errno)
 *   gj_form_get       → graph_batch197.c  ('+' → space form body)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch274_marker[] = "libcgj-batch274";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b274_hex_val — map ASCII hex digit to 0..15, or -1 if not hex.
 */
static int
b274_hex_val(unsigned char ch)
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
 * gj_pct_decode — strict RFC 3986-style percent-decode of a C string.
 *
 * in:     NUL-terminated percent-encoded input (required).
 * out:    destination buffer for decoded octets (required if any byte
 *         is produced, or always if out_cap > 0 and caller supplies it;
 *         required whenever decoded length would be > 0).
 * out_cap: capacity of out in bytes (not counting any sentinel).
 * out_n:  on success, set to the number of decoded bytes written.
 *
 * Semantics:
 *   - Literal bytes other than '%' copy as-is (including '+').
 *   - "%HH" with H hex (A-F/a-f/0-9) → one byte (hi<<4)|lo.
 *   - Truncated "%" / "%H" or non-hex → error.
 *   - Empty input succeeds with *out_n == 0 (out may be NULL if
 *     out_cap == 0).
 */
int
gj_pct_decode(const char *in, unsigned char *out, size_t out_cap,
    size_t *out_n)
{
	size_t iIn;
	size_t cbOut;
	unsigned char ch;
	int nHi;
	int nLo;

	if (in == NULL || out_n == NULL) {
		return -1;
	}

	iIn = 0u;
	cbOut = 0u;

	while (in[iIn] != '\0') {
		ch = (unsigned char)in[iIn];
		if (ch == (unsigned char)'%') {
			if (in[iIn + 1u] == '\0' || in[iIn + 2u] == '\0') {
				return -1;
			}
			nHi = b274_hex_val((unsigned char)in[iIn + 1u]);
			nLo = b274_hex_val((unsigned char)in[iIn + 2u]);
			if (nHi < 0 || nLo < 0) {
				return -1;
			}
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (unsigned char)((nHi << 4) | nLo);
			cbOut++;
			iIn += 3u;
		} else {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = ch;
			cbOut++;
			iIn++;
		}
	}

	*out_n = cbOut;
	return 0;
}

int __gj_pct_decode(const char *in, unsigned char *out, size_t out_cap,
    size_t *out_n)
    __attribute__((alias("gj_pct_decode")));
