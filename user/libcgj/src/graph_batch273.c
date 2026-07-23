/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch273: RFC 3986 percent-encode (unreserved pass).
 *
 * Surface (unique symbols):
 *   int gj_pct_encode(const unsigned char *in, size_t n,
 *                     char *out, size_t out_cap);
 *     — Percent-encode n octets from in into out (NUL-terminated).
 *       Unreserved octets (RFC 3986 §2.3: ALPHA DIGIT - . _ ~) are
 *       copied as-is; all other octets become %HH with uppercase hex
 *       digits (RFC 3986 §2.1). Space is %20 (not '+').
 *       out_cap includes the trailing NUL. Returns 0 on success, -1 on
 *       error (NULL out, NULL in with n != 0, or insufficient out_cap).
 *       Empty input (n == 0, in may be NULL) writes "" when out_cap >= 1.
 *   __gj_pct_encode  (alias)
 *   __libcgj_batch273_marker = "libcgj-batch273"
 *
 * Distinct from gj_url_encode (graph_batch103.c: C-string + errno).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch273_marker[] = "libcgj-batch273";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b273_is_unreserved — RFC 3986 §2.3 unreserved set:
 *   ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
static int
b273_is_unreserved(unsigned char uCh)
{
	if (uCh >= (unsigned char)'A' && uCh <= (unsigned char)'Z') {
		return 1;
	}
	if (uCh >= (unsigned char)'a' && uCh <= (unsigned char)'z') {
		return 1;
	}
	if (uCh >= (unsigned char)'0' && uCh <= (unsigned char)'9') {
		return 1;
	}
	if (uCh == (unsigned char)'-' || uCh == (unsigned char)'.' ||
	    uCh == (unsigned char)'_' || uCh == (unsigned char)'~') {
		return 1;
	}
	return 0;
}

/*
 * b273_hex_digit — nibble 0..15 → uppercase hex ASCII ('0'-'9', 'A'-'F').
 * RFC 3986 §2.1 recommends uppercase for percent-encoded octets.
 */
static char
b273_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + (char)uNib) : ('A' + (char)(uNib - 10u)));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pct_encode — RFC 3986 percent-encode all non-unreserved octets.
 *
 * in:       input byte buffer (NULL only legal when n == 0).
 * n:        number of input octets.
 * out:      destination C string buffer; must be non-NULL.
 * out_cap:  capacity of out including the terminating NUL.
 *
 * Each unreserved input octet is written as itself; every other octet
 * expands to three characters "%HH". The result is always NUL-terminated
 * on success.
 *
 * Edge cases:
 *   out == NULL                         → -1
 *   in == NULL && n != 0                → -1
 *   out_cap == 0                        → -1
 *   n == 0 (empty) with out_cap >= 1    → out = "", return 0
 *   encoded length + 1 > out_cap        → -1 (out undefined)
 */
int
gj_pct_encode(const unsigned char *in, size_t n, char *out, size_t out_cap)
{
	size_t iIn;
	size_t iOut;
	unsigned char uCh;

	if (out == NULL || out_cap == 0u) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	iOut = 0u;
	for (iIn = 0u; iIn < n; iIn++) {
		uCh = in[iIn];
		if (b273_is_unreserved(uCh)) {
			/* One literal octet + need room for final NUL. */
			if (iOut + 1u >= out_cap) {
				return -1;
			}
			out[iOut++] = (char)uCh;
		} else {
			/* "%HH" is three chars + need room for final NUL. */
			if (iOut + 3u >= out_cap) {
				return -1;
			}
			out[iOut++] = '%';
			out[iOut++] = b273_hex_digit((unsigned)uCh >> 4);
			out[iOut++] = b273_hex_digit((unsigned)uCh & 0x0fu);
		}
	}

	/* Room for NUL is guaranteed by the >= checks above (or empty n). */
	if (iOut >= out_cap) {
		return -1;
	}
	out[iOut] = '\0';
	return 0;
}

int __gj_pct_encode(const unsigned char *in, size_t n, char *out, size_t out_cap)
    __attribute__((alias("gj_pct_encode")));
