/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch338: DNS wire name → dotted presentation form.
 *
 * Surface (unique symbols):
 *   int gj_dns_name_decode(const uint8_t *in, size_t nin,
 *                          char *out, size_t out_cap);
 *     — Decode one RFC 1035 domain name from uncompressed wire form
 *       (length-prefixed labels ended by a root 0) into a NUL-terminated
 *       dotted presentation string. Compression pointers (top two bits
 *       of a length octet set: 0xC0 / reserved 0x40 / 0x80) are not
 *       followed; they are treated as errors (-1). Root name (single
 *       zero byte) becomes ".". Non-root names have no trailing dot.
 *       Returns the number of characters written to out (excluding the
 *       terminating NUL) on success; returns -1 on NULL args, truncated
 *       input, oversize label, oversize name, or insufficient out_cap.
 *   __gj_dns_name_decode  (alias)
 *   __libcgj_batch338_marker = "libcgj-batch338"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string. No third-party resolver source copied. Distinct from
 * ns_name_ntop / dn_expand (those live in earlier batches and may set
 * errno / follow compression with a message base).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch338_marker[] = "libcgj-batch338";

/* RFC 1035 §2.3.4 / §3.1 */
#define B338_MAX_LABEL  63u
#define B338_MAX_NAME   255u
#define B338_PTR_MASK   0xc0u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_name_decode — wire labels → dotted ASCII.
 *
 * in:      wire name buffer (must contain the full name including root 0)
 * nin:     number of readable bytes at in
 * out:     presentation buffer
 * out_cap: capacity of out including space for the terminating NUL
 *
 * On success out holds a NUL-terminated string and the return value is
 * its length (no NUL). On failure out is left unspecified and -1 is
 * returned (no errno).
 */
int
gj_dns_name_decode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
{
	size_t iIn;
	size_t iOut;
	size_t cbWire;
	unsigned uLab;
	unsigned iByte;
	int fFirst;

	if (in == NULL || out == NULL || out_cap == 0u) {
		return -1;
	}

	/* Root: single 0 → "." */
	if (nin >= 1u && in[0] == 0u) {
		if (out_cap < 2u) {
			return -1;
		}
		out[0] = '.';
		out[1] = '\0';
		return 1;
	}

	iIn = 0u;
	iOut = 0u;
	cbWire = 0u;
	fFirst = 1;

	while (iIn < nin) {
		uLab = (unsigned)in[iIn];

		/* End of name (root label) */
		if (uLab == 0u) {
			/*
			 * Need room for NUL. Non-root always wrote at least
			 * one label character before this terminator
			 * (root-only names take the early path above).
			 */
			if (iOut + 1u > out_cap) {
				return -1;
			}
			out[iOut] = '\0';
			return (int)iOut;
		}

		/*
		 * Top two bits of the length octet:
		 *   11 = compression pointer, 01/10 = reserved.
		 * This decoder does not follow pointers (no message base);
		 * any non-zero type bits are an error. With those bits
		 * clear, the remaining six bits are the label length
		 * (0..63); 0 was handled above as root.
		 */
		if ((uLab & B338_PTR_MASK) != 0u) {
			return -1;
		}
		if (uLab > B338_MAX_LABEL) {
			return -1;
		}

		/* Length octet + label + eventual root 0 must fit wire max */
		if (cbWire + 1u + (size_t)uLab + 1u > B338_MAX_NAME) {
			return -1;
		}

		/* Consume length octet */
		iIn++;
		cbWire++;

		if (iIn + (size_t)uLab > nin) {
			/* Truncated label payload */
			return -1;
		}

		/* Dot separator between labels (not before first) */
		if (!fFirst) {
			if (iOut + 1u >= out_cap) {
				return -1;
			}
			out[iOut] = '.';
			iOut++;
		}
		fFirst = 0;

		/* Copy label octets as-is (presentation characters) */
		for (iByte = 0u; iByte < uLab; iByte++) {
			if (iOut + 1u >= out_cap) {
				return -1;
			}
			out[iOut] = (char)in[iIn + (size_t)iByte];
			iOut++;
		}
		iIn += (size_t)uLab;
		cbWire += (size_t)uLab;
	}

	/* Ran out of input without a root terminator */
	return -1;
}

int __gj_dns_name_decode(const uint8_t *in, size_t nin, char *out,
    size_t out_cap) __attribute__((alias("gj_dns_name_decode")));
