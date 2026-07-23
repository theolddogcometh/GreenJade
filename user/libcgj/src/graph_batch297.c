/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch297: RFC 4648 base32 decode (standard alphabet).
 *
 * Surface (unique symbols — does NOT redefine base32_decode / base32hex_*):
 *   int gj_b32_decode(const char *in, uint8_t *out, size_t out_cap,
 *                     size_t *nout);
 *     — Decode NUL-terminated RFC 4648 base32 text (alphabet A-Z2-7,
 *       case-insensitive; '=' padding) into binary out[0..out_cap).
 *       On success *nout = decoded byte count and return is 0.
 *       Empty input succeeds with *nout = 0.
 *       Returns -1 on error (NULL in/nout, NULL out when bytes needed,
 *       out_cap too small, invalid alphabet, bad padding, or truncated
 *       final group). *nout is undefined on error.
 *   __gj_b32_decode  (alias)
 *   __libcgj_batch297_marker = "libcgj-batch297"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   base32_encode / base32_decode       → graph_batch41.c (ssize_t + errno)
 *   base32hex_encode / base32hex_decode → graph_batch42.c (RFC 4648 §7)
 *   crockford32_*                       → graph_batch119.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch297_marker[] = "libcgj-batch297";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b297_b32_val — map one RFC 4648 base32 digit to 0..31, or -1 if invalid.
 *
 * Alphabet (RFC 4648 §6): A-Z → 0..25, 2-7 → 26..31.
 * Lowercase a-z is accepted as an implementation convenience (same values).
 * Whitespace and other punctuation are rejected.
 */
static int
b297_b32_val(unsigned char ch)
{
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') {
		return (int)(ch - (unsigned char)'A');
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'z') {
		return (int)(ch - (unsigned char)'a');
	}
	if (ch >= (unsigned char)'2' && ch <= (unsigned char)'7') {
		return (int)(ch - (unsigned char)'2') + 26;
	}
	return -1;
}

/*
 * b297_pad_to_nouts — map pad count in a final 8-char group to payload bytes.
 *
 * RFC 4648 quanta (input bytes → pad chars → output bytes of that quantum):
 *   5 → 0 pads → 5 out
 *   4 → 1 pad  → 4 out
 *   3 → 3 pads → 3 out
 *   2 → 4 pads → 2 out
 *   1 → 6 pads → 1 out
 * Any other pad count is illegal.
 * Returns output byte count, or -1 if nPad is not a legal RFC value.
 */
static int
b297_pad_to_nouts(int nPad)
{
	if (nPad == 0) {
		return 5;
	}
	if (nPad == 1) {
		return 4;
	}
	if (nPad == 3) {
		return 3;
	}
	if (nPad == 4) {
		return 2;
	}
	if (nPad == 6) {
		return 1;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b32_decode — RFC 4648 base32 decode of a NUL-terminated C string.
 *
 * in:      NUL-terminated base32 text (required).
 * out:     destination buffer (required whenever decoded length > 0;
 *          may be NULL only for empty successful decode with out_cap == 0
 *          or when no bytes are produced).
 * out_cap: capacity of out in bytes.
 * nout:    on success, set to the number of decoded bytes written (required).
 *
 * Semantics:
 *   - Alphabet A-Z / a-z / 2-7; '=' only as trailing padding of a group.
 *   - Input is consumed in 8-character groups (40 bits → up to 5 bytes).
 *   - Padding may only appear at the end of a group, and only in the
 *     final group; once a pad is seen, remaining group slots must be '='.
 *   - No whitespace or other separators.
 *   - Empty string → success, *nout = 0.
 *
 * Returns 0 on success, -1 on error.
 */
int
gj_b32_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
{
	size_t iIn;
	size_t cbOut;
	int fDone;

	if (in == NULL || nout == NULL) {
		return -1;
	}

	iIn = 0u;
	cbOut = 0u;
	fDone = 0;

	while (in[iIn] != '\0') {
		int aV[8];
		int k;
		int nPad;
		int nOut;
		uint64_t uBits;

		if (fDone != 0) {
			/* Trailing garbage after a padded final group. */
			return -1;
		}

		/*
		 * Collect one 8-character quantum. End-of-string mid-group
		 * without completing eight slots is a truncated encoding.
		 */
		nPad = 0;
		for (k = 0; k < 8; k++) {
			unsigned char ch;

			if (in[iIn] == '\0') {
				return -1;
			}
			ch = (unsigned char)in[iIn];
			iIn++;

			if (ch == (unsigned char)'=') {
				if (k < 2) {
					/* Need at least two data digits. */
					return -1;
				}
				aV[k] = 0;
				nPad++;
			} else {
				int nVal;

				if (nPad > 0) {
					/* Data digit after pad in this group. */
					return -1;
				}
				nVal = b297_b32_val(ch);
				if (nVal < 0) {
					return -1;
				}
				aV[k] = nVal;
			}
		}

		nOut = b297_pad_to_nouts(nPad);
		if (nOut < 0) {
			return -1;
		}
		if (nPad > 0) {
			/* Padded group must be the last quantum. */
			fDone = 1;
		}

		/* Pack eight 5-bit symbols into a 40-bit big-endian value. */
		uBits = 0u;
		for (k = 0; k < 8; k++) {
			uBits = (uBits << 5) | (uint64_t)(unsigned)aV[k];
		}

		/*
		 * Emit nOut bytes from the high end of the 40-bit buffer.
		 * Capacity and out pointer are checked before each store.
		 */
		if (nOut >= 1) {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (uint8_t)(uBits >> 32);
			cbOut++;
		}
		if (nOut >= 2) {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (uint8_t)(uBits >> 24);
			cbOut++;
		}
		if (nOut >= 3) {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (uint8_t)(uBits >> 16);
			cbOut++;
		}
		if (nOut >= 4) {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (uint8_t)(uBits >> 8);
			cbOut++;
		}
		if (nOut >= 5) {
			if (cbOut >= out_cap || out == NULL) {
				return -1;
			}
			out[cbOut] = (uint8_t)uBits;
			cbOut++;
		}
	}

	*nout = cbOut;
	return 0;
}

int __gj_b32_decode(const char *in, uint8_t *out, size_t out_cap, size_t *nout)
    __attribute__((alias("gj_b32_decode")));
