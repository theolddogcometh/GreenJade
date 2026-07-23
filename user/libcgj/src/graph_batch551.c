/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch551: freestanding URL-safe base64 encode
 * (RFC 4648 §5 alphabet, no '=' padding).
 *
 * Surface (unique symbols — does NOT redefine existing base64 APIs):
 *   int gj_base64url_enc(const unsigned char *in, size_t n,
 *                        char *out, size_t cap);
 *     — Encode n octets from in into out as URL-safe base64
 *       (alphabet A-Za-z0-9-_, no padding). Writes a NUL-terminated
 *       string. Returns the number of characters written excluding
 *       the trailing NUL on success, or -1 on error (NULL in when
 *       n > 0, NULL out, cap too small, or length not representable
 *       as int).
 *   int __gj_base64url_enc  (alias)
 *   __libcgj_batch551_marker = "libcgj-batch551"
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   base64_encode / base64_decode           → graph_batch40.c
 *   base64url_encode / base64url_decode     → graph_batch93.c
 *   gj_b64url_encode_pad / _decode_pad      → graph_batch186.c
 *   gj_jwt_b64url_decode_seg                → graph_batch162.c
 *   gj_b64url_decode                        → graph_batch331.c
 *   b64_ntop / b64_pton                     → graph_batch22.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch551_marker[] = "libcgj-batch551";

/* RFC 4648 §5 base64url alphabet (URL-safe; no '+' '/'). */
static const char s_b551_alpha[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/* Largest positive int on typical 32-bit int hosts (return is int). */
#define B551_INT_MAX 2147483647

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base64url_enc — unpadded URL-safe base64 encode into caller buffer.
 *
 * in:  source octets; may be NULL only when n == 0.
 * n:   number of input octets.
 * out: destination buffer (must be non-NULL); receives NUL-terminated
 *      encoded text (no '=' padding).
 * cap: capacity of out including room for the terminating NUL.
 *
 * Encoding length (no pad, no NUL):
 *   4 * (n / 3) + { 0 if n%3==0; 2 if n%3==1; 3 if n%3==2 }
 *
 * Returns encoded character count excluding NUL, or -1 on error.
 *
 * Examples:
 *   ""    / 0 → ""     (return 0)
 *   "f"   / 1 → "Zg"   (return 2)
 *   "fo"  / 2 → "Zm8"  (return 3)
 *   "foo" / 3 → "Zm9v" (return 4)
 */
int
gj_base64url_enc(const unsigned char *in, size_t n, char *out, size_t cap)
{
	size_t i;
	size_t o;
	size_t nFull;
	size_t nRem;
	size_t cbNeed;

	if (out == NULL) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	nFull = n / 3u;
	nRem = n % 3u;

	/* Overflow-safe need: 4*nFull + rem tail (2 or 3) + NUL. */
	if (nFull > ((size_t)-1) / 4u) {
		return -1;
	}
	cbNeed = nFull * 4u;
	if (nRem == 1u) {
		if (cbNeed > (size_t)-1 - 2u) {
			return -1;
		}
		cbNeed += 2u;
	} else if (nRem == 2u) {
		if (cbNeed > (size_t)-1 - 3u) {
			return -1;
		}
		cbNeed += 3u;
	}
	/* Need one more byte for trailing NUL. */
	if (cbNeed == (size_t)-1) {
		return -1;
	}
	if (cbNeed + 1u > cap) {
		return -1;
	}
	if (cbNeed > (size_t)B551_INT_MAX) {
		return -1;
	}

	o = 0u;
	i = 0u;
	while (i + 3u <= n) {
		unsigned uTrip = ((unsigned)in[i] << 16) |
		                 ((unsigned)in[i + 1u] << 8) |
		                 (unsigned)in[i + 2u];

		out[o++] = s_b551_alpha[(uTrip >> 18) & 63u];
		out[o++] = s_b551_alpha[(uTrip >> 12) & 63u];
		out[o++] = s_b551_alpha[(uTrip >> 6) & 63u];
		out[o++] = s_b551_alpha[uTrip & 63u];
		i += 3u;
	}

	if (nRem == 1u) {
		unsigned uTrip = (unsigned)in[i] << 16;

		out[o++] = s_b551_alpha[(uTrip >> 18) & 63u];
		out[o++] = s_b551_alpha[(uTrip >> 12) & 63u];
		/* no padding */
	} else if (nRem == 2u) {
		unsigned uTrip = ((unsigned)in[i] << 16) |
		                 ((unsigned)in[i + 1u] << 8);

		out[o++] = s_b551_alpha[(uTrip >> 18) & 63u];
		out[o++] = s_b551_alpha[(uTrip >> 12) & 63u];
		out[o++] = s_b551_alpha[(uTrip >> 6) & 63u];
		/* no padding */
	}

	out[o] = '\0';
	return (int)o;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_base64url_enc(const unsigned char *in, size_t n, char *out,
                       size_t cap)
    __attribute__((alias("gj_base64url_enc")));
