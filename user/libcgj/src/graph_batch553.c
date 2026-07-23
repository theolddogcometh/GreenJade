/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch553: length-bounded RFC 3986 URL percent-encode.
 *
 * Surface (unique symbols):
 *   int gj_url_encode_n(const unsigned char *in, size_t n,
 *                       char *out, size_t cap);
 *     — Percent-encode n octets from in into out (NUL-terminated).
 *       RFC 3986 §2.3 unreserved octets (ALPHA DIGIT - . _ ~) pass
 *       through; all other octets become %HH with uppercase hex digits
 *       (RFC 3986 §2.1). Space is %20 (not '+').
 *       cap includes the trailing NUL. Returns encoded length excluding
 *       NUL on success, or -1 on error.
 *   __gj_url_encode_n  (alias)
 *   __libcgj_batch553_marker = "libcgj-batch553"
 *
 * Existing surface NOT redefined (avoid multi-def with urlencode):
 *   gj_url_encode / __gj_url_encode   → graph_batch103.c  (C-string + errno)
 *   gj_pct_encode / __gj_pct_encode   → graph_batch273.c  (0/-1, not length)
 *   gj_url_decode / gj_pct_decode     → batch103 / batch274
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch553_marker[] = "libcgj-batch553";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b553_is_unreserved — RFC 3986 §2.3 unreserved set:
 *   ALPHA / DIGIT / "-" / "." / "_" / "~"
 */
static int
b553_is_unreserved(unsigned char uCh)
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
 * b553_hex_digit — nibble 0..15 → uppercase hex ASCII ('0'-'9', 'A'-'F').
 * RFC 3986 §2.1 recommends uppercase for percent-encoded octets.
 */
static char
b553_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + (char)uNib) : ('A' + (char)(uNib - 10u)));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_url_encode_n — RFC 3986 percent-encode of an explicit-length buffer.
 *
 * in:   input byte buffer (NULL only legal when n == 0).
 * n:    number of input octets.
 * out:  destination C string buffer; must be non-NULL.
 * cap:  capacity of out including the terminating NUL.
 *
 * Each unreserved input octet is written as itself; every other octet
 * expands to three characters "%HH". The result is always NUL-terminated
 * on success. Return value is the encoded length excluding the NUL.
 *
 * Edge cases:
 *   out == NULL                         → -1
 *   in == NULL && n != 0                → -1
 *   cap == 0                            → -1
 *   n == 0 (empty) with cap >= 1        → out = "", return 0
 *   encoded length + 1 > cap            → -1 (out undefined)
 */
int
gj_url_encode_n(const unsigned char *in, size_t n, char *out, size_t cap)
{
	size_t iIn;
	size_t iOut;
	unsigned char uCh;

	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	iOut = 0u;
	for (iIn = 0u; iIn < n; iIn++) {
		uCh = in[iIn];
		if (b553_is_unreserved(uCh)) {
			/* One literal octet + need room for final NUL. */
			if (iOut + 1u >= cap) {
				return -1;
			}
			out[iOut++] = (char)uCh;
		} else {
			/* "%HH" is three chars + need room for final NUL. */
			if (iOut + 3u >= cap) {
				return -1;
			}
			out[iOut++] = '%';
			out[iOut++] = b553_hex_digit((unsigned)uCh >> 4);
			out[iOut++] = b553_hex_digit((unsigned)uCh & 0x0fu);
		}
	}

	/* Room for NUL is guaranteed by the >= checks above (or empty n). */
	if (iOut >= cap) {
		return -1;
	}
	out[iOut] = '\0';
	return (int)iOut;
}

int __gj_url_encode_n(const unsigned char *in, size_t n, char *out, size_t cap)
    __attribute__((alias("gj_url_encode_n")));
