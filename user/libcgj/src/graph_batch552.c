/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch552: RFC 4648 §5 base64url decode (URL-safe).
 *
 * Surface (unique symbols):
 *   int gj_base64url_dec(const char *in, size_t n, unsigned char *out,
 *                        size_t cap);
 *     — Decode n bytes of base64url from in into out (capacity cap).
 *       Alphabet: A-Za-z0-9-_ . Optional '=' padding is ignored.
 *       Returns decoded byte count, or -1 on error (NULL/invalid/ENOSPC).
 *   int __gj_base64url_dec  (alias)
 *   __libcgj_batch552_marker = "libcgj-batch552"
 *
 * Distinct from base64url_decode in graph_batch93.c (ssize_t / errno API).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch552_marker[] = "libcgj-batch552";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map one base64url alphabet byte to its 6-bit value, or -1 if not in
 * the RFC 4648 §5 alphabet (A-Za-z0-9-_). '=' is not mapped here.
 */
static int
b552_val(unsigned char c)
{
	if (c >= (unsigned char)'A' && c <= (unsigned char)'Z') {
		return (int)(c - (unsigned char)'A');
	}
	if (c >= (unsigned char)'a' && c <= (unsigned char)'z') {
		return (int)(c - (unsigned char)'a') + 26;
	}
	if (c >= (unsigned char)'0' && c <= (unsigned char)'9') {
		return (int)(c - (unsigned char)'0') + 52;
	}
	if (c == (unsigned char)'-') {
		return 62;
	}
	if (c == (unsigned char)'_') {
		return 63;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64url_dec — decode URL-safe base64 into out.
 *
 * in:  input buffer of length n (may be NULL only when n == 0)
 * n:   number of input bytes to consume
 * out: destination of at least cap bytes (may be NULL only if result is 0)
 * cap: capacity of out in bytes
 *
 * Returns:
 *   >= 0  number of decoded octets written to out
 *   -1    on NULL/invalid alphabet/lone tail digit/cap overflow
 *
 * Optional '=' padding characters are skipped and do not contribute to
 * the digit count. Unpadded tails of 2 or 3 alphabet digits are accepted
 * (1 leftover alphabet digit is invalid). Does not call libc or set errno.
 */
int
gj_base64url_dec(const char *in, size_t n, unsigned char *out, size_t cap)
{
	size_t i;
	size_t o;
	unsigned acc;
	unsigned nbits;
	unsigned ndig;

	if (n == 0u) {
		return 0;
	}
	if (in == NULL) {
		return -1;
	}

	o = 0u;
	acc = 0u;
	nbits = 0u;
	ndig = 0u;

	for (i = 0u; i < n; i++) {
		unsigned char c;
		int v;

		c = (unsigned char)in[i];
		if (c == (unsigned char)'=') {
			/* Optional padding — ignore. */
			continue;
		}
		v = b552_val(c);
		if (v < 0) {
			return -1;
		}

		acc = (acc << 6) | (unsigned)v;
		nbits += 6u;
		ndig++;

		if (nbits >= 8u) {
			nbits -= 8u;
			if (out == NULL || o >= cap) {
				return -1;
			}
			out[o++] = (unsigned char)((acc >> nbits) & 0xffu);
		}
	}

	/* 1 leftover alphabet digit cannot form a full octet group. */
	if ((ndig % 4u) == 1u) {
		return -1;
	}

	/* Remaining low bits from a 2- or 3-digit tail must be discarded
	 * (they are padding bits in the last sextet). nbits is 0, 2, or 4
	 * for valid ndig % 4 in {0, 3, 2}; no extra emission.
	 */
	(void)acc;
	(void)nbits;

	if (o > (size_t)0x7fffffff) {
		return -1;
	}
	return (int)o;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base64url_dec(const char *in, size_t n, unsigned char *out, size_t cap)
    __attribute__((alias("gj_base64url_dec")));
