/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch627: strict base16 (hex) decode of n bytes.
 *
 * Surface (unique symbols — do NOT collide with hex_decode / gj_hex_decode):
 *   int gj_base16_dec(const char *in, size_t n, unsigned char *out,
 *                     size_t cap);
 *     — Decode n hex ASCII digits from in into out (capacity cap).
 *       Digits are case-insensitive ('0'-'9', 'a'-'f', 'A'-'F').
 *       n must be even. No whitespace or separators.
 *       Returns decoded byte count, or -1 on error (NULL/odd n/invalid
 *       digit/cap overflow / result not representable as int).
 *   int __gj_base16_dec  (alias)
 *   __libcgj_batch627_marker = "libcgj-batch627"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   hex_decode    → graph_batch41.c  (ssize_t, errno, whitespace)
 *   gj_hex_decode → graph_batch298.c (NUL-terminated, nout pointer)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch627_marker[] = "libcgj-batch627";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b627_hex_val — map ASCII hex digit to 0..15, or -1 if not hex.
 */
static int
b627_hex_val(unsigned char ch)
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
 * gj_base16_dec — strict base16 decode of an n-byte hex slice.
 *
 * in:  input buffer of length n (may be NULL only when n == 0)
 * n:   number of hex digits to consume (must be even)
 * out: destination of at least cap bytes (may be NULL only if result is 0)
 * cap: capacity of out in bytes
 *
 * Returns:
 *   >= 0  number of decoded octets written to out (n/2 on success)
 *   -1    on NULL/odd n/non-hex digit/cap overflow
 *
 * Empty input (n == 0) succeeds with return 0 (out may be NULL).
 * Does not call libc or set errno.
 */
int
gj_base16_dec(const char *in, size_t n, unsigned char *out, size_t cap)
{
	size_t i;
	size_t nbytes;
	size_t o;

	if (n == 0u) {
		return 0;
	}
	if (in == NULL) {
		return -1;
	}
	/* n must be even. */
	if ((n & 1u) != 0u) {
		return -1;
	}

	nbytes = n / 2u;
	if (nbytes > cap) {
		return -1;
	}
	if (out == NULL) {
		return -1;
	}
	if (nbytes > (size_t)0x7fffffff) {
		return -1;
	}

	o = 0u;
	for (i = 0u; i < n; i += 2u) {
		int nHi;
		int nLo;

		nHi = b627_hex_val((unsigned char)in[i]);
		nLo = b627_hex_val((unsigned char)in[i + 1u]);
		if (nHi < 0 || nLo < 0) {
			return -1;
		}
		out[o++] = (unsigned char)(((unsigned)nHi << 4) | (unsigned)nLo);
	}

	return (int)o;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base16_dec(const char *in, size_t n, unsigned char *out, size_t cap)
    __attribute__((alias("gj_base16_dec")));
