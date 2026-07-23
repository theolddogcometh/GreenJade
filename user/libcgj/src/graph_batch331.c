/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch331: RFC 4648 §5 base64url decode (URL-safe).
 *
 * Surface (unique symbols — does NOT redefine base64url_decode /
 * gj_b64url_decode_pad / base64_decode):
 *   int gj_b64url_decode(const char *in, uint8_t *out, size_t out_cap,
 *                        size_t *nout);
 *     — Decode NUL-terminated RFC 4648 URL-safe base64 text
 *       (alphabet A-Za-z0-9-_, optional '=' padding; unpadded tails of
 *       2 or 3 alphabet digits accepted) into binary out[0..out_cap).
 *       On success *nout = decoded byte count and return is 0.
 *       Empty input succeeds with *nout = 0.
 *       Returns -1 on error (NULL in/nout, NULL out when bytes needed,
 *       out_cap too small, invalid alphabet, bad padding, or truncated
 *       final group). *nout is undefined on error.
 *   __gj_b64url_decode  (alias)
 *   __libcgj_batch331_marker = "libcgj-batch331"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base64_encode / base64_decode           → graph_batch40.c
 *   base64url_encode / base64url_decode     → graph_batch93.c
 *   gj_b64url_encode_pad / decode_pad       → graph_batch186.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch331_marker[] = "libcgj-batch331";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b331_b64url_val — map one RFC 4648 §5 base64url digit to 0..63, or -1.
 *
 * Alphabet: A-Z → 0..25, a-z → 26..51, 0-9 → 52..61, '-' → 62, '_' → 63.
 * Standard base64 '+' / '/' are rejected (URL-safe only).
 */
static int
b331_b64url_val(unsigned char ch)
{
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
		return (int)(ch - (unsigned char)'A');
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'z') {
		return (int)(ch - (unsigned char)'a') + 26;
	}
	if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') {
		return (int)(ch - (unsigned char)'0') + 52;
	}
	if (ch == (unsigned char)'-') {
		return 62;
	}
	if (ch == (unsigned char)'_') {
		return 63;
	}
	return -1;
}

/*
 * b331_emit — write nOut (1..3) payload bytes from a 24-bit triple.
 *
 * Returns 0 on success, -1 if out is NULL when bytes are needed or
 * out_cap is insufficient.
 */
static int
b331_emit(unsigned uTrip, int nOut, uint8_t *out, size_t out_cap,
          size_t *pcbOut)
{
	size_t j = *pcbOut;

	if (nOut < 1 || nOut > 3) {
		return -1;
	}
	if (out == NULL || j + (size_t)nOut > out_cap) {
		return -1;
	}

	out[j++] = (uint8_t)(uTrip >> 16);
	if (nOut > 1) {
		out[j++] = (uint8_t)(uTrip >> 8);
	}
	if (nOut > 2) {
		out[j++] = (uint8_t)uTrip;
	}
	*pcbOut = j;
	return 0;
}

/*
 * b331_decode_quartet — decode one logical 4-char base64url group.
 *
 * aC[0..3] are alphabet digits or '='. Returns 0 / -1.
 */
static int
b331_decode_quartet(const char aC[4], uint8_t *out, size_t out_cap,
                    size_t *pcbOut)
{
	int v0, v1, v2, v3;
	unsigned uTrip;
	int nOut;
	char c2 = aC[2];
	char c3 = aC[3];

	v0 = b331_b64url_val((unsigned char)aC[0]);
	v1 = b331_b64url_val((unsigned char)aC[1]);
	if (v0 < 0 || v1 < 0) {
		return -1;
	}

	if (c2 == '=') {
		/* "xx==" → 1 payload byte; c3 must also be '=' */
		if (c3 != '=') {
			return -1;
		}
		v2 = 0;
		v3 = 0;
		nOut = 1;
	} else {
		v2 = b331_b64url_val((unsigned char)c2);
		if (v2 < 0) {
			return -1;
		}
		if (c3 == '=') {
			/* "xxx=" → 2 payload bytes */
			v3 = 0;
			nOut = 2;
		} else {
			v3 = b331_b64url_val((unsigned char)c3);
			if (v3 < 0) {
				return -1;
			}
			nOut = 3;
		}
	}

	uTrip = ((unsigned)v0 << 18) | ((unsigned)v1 << 12) |
	        ((unsigned)v2 << 6) | (unsigned)v3;
	return b331_emit(uTrip, nOut, out, out_cap, pcbOut);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64url_decode — RFC 4648 §5 base64url decode of a NUL-terminated string.
 *
 * in:      NUL-terminated base64url text (required).
 * out:     destination buffer (required whenever decoded length > 0;
 *          may be NULL only for empty successful decode).
 * out_cap: capacity of out in bytes.
 * nout:    on success, set to the number of decoded bytes written (required).
 *
 * Semantics:
 *   - Alphabet A-Z / a-z / 0-9 / '-' / '_'; '=' only as trailing padding.
 *   - Input is consumed in 4-character groups (24 bits → up to 3 bytes).
 *   - Optional '=' padding; unpadded tails of 2 or 3 alphabet digits
 *     are accepted (JOSE / RFC 4648 unpadded practice). A single leftover
 *     digit is invalid.
 *   - No whitespace or other separators; standard base64 +/ rejected.
 *   - Empty string → success, *nout = 0.
 *
 * Returns 0 on success, -1 on error.
 */
int
gj_b64url_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
{
	size_t iIn;
	size_t cbOut;
	char aQ[4];
	unsigned nQ;
	int fPadSeen;

	if (in == NULL || nout == NULL) {
		return -1;
	}

	iIn = 0u;
	cbOut = 0u;
	nQ = 0u;
	fPadSeen = 0;

	while (in[iIn] != '\0') {
		unsigned char ch = (unsigned char)in[iIn];

		iIn++;

		if (fPadSeen != 0) {
			/* Trailing garbage after a completed padded quartet. */
			return -1;
		}

		if (ch == (unsigned char)'=') {
			if (nQ < 2u) {
				/* Need at least two data digits before pad. */
				return -1;
			}
			aQ[nQ++] = '=';
			if (nQ == 4u) {
				if (b331_decode_quartet(aQ, out, out_cap,
				                        &cbOut) != 0) {
					return -1;
				}
				nQ = 0u;
				fPadSeen = 1;
			}
			continue;
		}

		if (b331_b64url_val(ch) < 0) {
			return -1;
		}
		/* Alphabet digit cannot follow pad within a quartet. */
		if (nQ > 0u && aQ[nQ - 1u] == '=') {
			return -1;
		}
		aQ[nQ++] = (char)ch;
		if (nQ == 4u) {
			if (b331_decode_quartet(aQ, out, out_cap, &cbOut) !=
			    0) {
				return -1;
			}
			nQ = 0u;
		}
	}

	/* Unpadded tail: 0, 2, or 3 alphabet digits (1 is invalid). */
	if (nQ == 0u) {
		*nout = cbOut;
		return 0;
	}
	if (nQ == 1u) {
		return -1;
	}
	/* Incomplete padded quartet (e.g. only one '=' of "==") is invalid. */
	{
		unsigned k;

		for (k = 0u; k < nQ; k++) {
			if (aQ[k] == '=') {
				return -1;
			}
		}
	}
	if (nQ == 2u) {
		/* "xx" → one byte; pad virtually as "xx==" */
		aQ[2] = '=';
		aQ[3] = '=';
	} else {
		/* nQ == 3: "xxx" → two bytes; pad virtually as "xxx=" */
		aQ[3] = '=';
	}
	if (b331_decode_quartet(aQ, out, out_cap, &cbOut) != 0) {
		return -1;
	}

	*nout = cbOut;
	return 0;
}

int __gj_b64url_decode(const char *in, uint8_t *out, size_t out_cap,
                       size_t *nout)
    __attribute__((alias("gj_b64url_decode")));
