/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch626: lowercase base16 (hex) encode under a
 * unique gj_ name that returns the encoded character length.
 *
 * Surface (unique symbols — do NOT collide with hex_encode / gj_hex_encode):
 *   int gj_base16_enc(const unsigned char *in, size_t n, char *out,
 *                     size_t cap);
 *     — Encode n bytes at in as lowercase hex ('0'-'9', 'a'-'f') into out.
 *       Writes 2*n ASCII digits plus a terminating NUL. out must have room
 *       for 2*n + 1 bytes (cap includes the NUL). Returns the encoded length
 *       2*n on success, or -1 on error.
 *       Empty input (n == 0, in may be NULL) writes "" when cap >= 1 and
 *       returns 0.
 *   __gj_base16_enc  (alias)
 *   __libcgj_batch626_marker = "libcgj-batch626"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   hex_encode / hex_decode / __hex_encode     → graph_batch41.c (ssize_t)
 *   gj_hex_encode / gj_hex_decode / __gj_hex_* → graph_batch298.c (0/-1)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch626_marker[] = "libcgj-batch626";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b626_hex_digit — nibble 0..15 → lowercase hex ASCII ('0'-'9', 'a'-'f').
 */
static char
b626_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + (char)uNib)
	                           : ('a' + (char)(uNib - 10u)));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base16_enc — lowercase base16 of n octets into a C string.
 *
 * in:  input bytes (NULL only legal when n == 0)
 * n:   input length in bytes
 * out: destination C string buffer (must be non-NULL)
 * cap: capacity of out including the terminating NUL
 *
 * Edge cases:
 *   out == NULL                         → -1
 *   in == NULL && n != 0                → -1
 *   cap == 0                            → -1
 *   n == 0 (empty) with cap >= 1        → out = "", return 0
 *   2*n + 1 > cap (or size_t overflow)  → -1 (out undefined)
 *   2*n does not fit in int             → -1
 *
 * On success returns 2*n (encoded character count, not including NUL).
 */
int
gj_base16_enc(const unsigned char *in, size_t n, char *out, size_t cap)
{
	size_t i;
	size_t nEnc;
	size_t nNeed;

	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	/*
	 * Need 2*n + 1 bytes (digits + NUL). Guard against size_t
	 * overflow when n is near SIZE_MAX / 2.
	 */
	if (n > ((size_t)-1 - 1u) / 2u) {
		return -1;
	}
	nEnc = n * 2u;
	nNeed = nEnc + 1u;
	if (cap < nNeed) {
		return -1;
	}

	/*
	 * Return type is int: reject lengths that do not fit in a
	 * non-negative int (portable 2's-complement INT_MAX bound).
	 */
	if (nEnc > (size_t)(((unsigned)-1) >> 1)) {
		return -1;
	}

	for (i = 0u; i < n; i++) {
		unsigned char uByte = in[i];

		out[i * 2u] = b626_hex_digit((unsigned)(uByte >> 4) & 0x0fu);
		out[i * 2u + 1u] = b626_hex_digit((unsigned)uByte & 0x0fu);
	}
	out[nEnc] = '\0';
	return (int)nEnc;
}

int __gj_base16_enc(const unsigned char *in, size_t n, char *out, size_t cap)
    __attribute__((alias("gj_base16_enc")));
