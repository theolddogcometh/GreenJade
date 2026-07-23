/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch296: RFC 4648 base32 encode under unique gj_ names.
 *
 * Surface (unique symbols — does not redefine base32_encode / __base32_encode):
 *   int gj_b32_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap);
 *     — RFC 4648 §6 base32 encode of nin octets at in into out.
 *       Alphabet A-Z2-7 with canonical '=' padding to a multiple of 8.
 *       Writes a NUL-terminated string when out_cap is large enough.
 *       Returns encoded length excluding NUL on success, -1 on error
 *       (NULL out, NULL in with nin != 0, out_cap too small, or length
 *       not representable as int).
 *       Empty input (nin == 0, in may be NULL) writes "" when out_cap >= 1
 *       and returns 0.
 *   __gj_b32_encode  (alias)
 *   __libcgj_batch296_marker = "libcgj-batch296"
 *
 * Distinct from:
 *   base32_encode / base32_decode           → graph_batch41.c (RFC 4648)
 *   base32hex_encode / base32hex_decode     → graph_batch42.c (RFC 4648 §7)
 *   crockford base32                        → graph_batch119.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch296_marker[] = "libcgj-batch296";

/* RFC 4648 §6 base32 alphabet (A-Z then 2-7). */
static const char s_b296_b32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b32_encode — RFC 4648 §6 base32 encode with canonical padding.
 *
 * in:       input octets (NULL only legal when nin == 0).
 * nin:      number of input octets.
 * out:      destination C string buffer; must be non-NULL.
 * out_cap:  capacity of out including the terminating NUL.
 *
 * Encoding walks the input in 5-byte quanta. Each full quantum emits 8
 * alphabet characters. A partial final quantum is zero-padded for bit
 * packing and finished with '=' so the group length is always 8:
 *   leftover 1 → 2 data + 6 pad
 *   leftover 2 → 4 data + 4 pad
 *   leftover 3 → 5 data + 3 pad
 *   leftover 4 → 7 data + 1 pad
 *   leftover 5 → 8 data + 0 pad
 *
 * Edge cases:
 *   out == NULL                         → -1
 *   in == NULL && nin != 0              → -1
 *   out_cap == 0                        → -1
 *   nin == 0 with out_cap >= 1          → out = "", return 0
 *   encoded length + 1 > out_cap        → -1
 *   encoded length > INT_MAX            → -1
 */
int
gj_b32_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
{
	size_t iIn;
	size_t iOut;
	size_t nGroups;
	size_t cbEnc;
	size_t cbNeed;

	if (out == NULL || out_cap == 0u) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}

	/* Empty → empty C string. */
	if (nin == 0u) {
		out[0] = '\0';
		return 0;
	}

	/*
	 * Output length: ceil(nin / 5) groups of 8 chars, plus trailing NUL.
	 * nGroups = (nin + 4) / 5; overflow-safe multiply check below.
	 */
	nGroups = (nin + 4u) / 5u;
	if (nGroups > ((size_t)-1) / 8u) {
		return -1;
	}
	cbEnc = nGroups * 8u;
	if (cbEnc > ((size_t)-1) - 1u) {
		return -1;
	}
	cbNeed = cbEnc + 1u;
	if (cbNeed > out_cap) {
		return -1;
	}
	/* Return value is int: reject lengths that do not fit. */
	if (cbEnc > (size_t)0x7fffffff) {
		return -1;
	}

	iIn = 0u;
	iOut = 0u;
	while (iIn < nin) {
		size_t nLeft = nin - iIn;
		unsigned u0 = in[iIn++];
		unsigned u1 = (nLeft > 1u) ? in[iIn++] : 0u;
		unsigned u2 = (nLeft > 2u) ? in[iIn++] : 0u;
		unsigned u3 = (nLeft > 3u) ? in[iIn++] : 0u;
		unsigned u4 = (nLeft > 4u) ? in[iIn++] : 0u;
		/* 40-bit buffer in the low 40 bits of u64. */
		uint64_t u64 =
		    ((uint64_t)u0 << 32) | ((uint64_t)u1 << 24) |
		    ((uint64_t)u2 << 16) | ((uint64_t)u3 << 8) | (uint64_t)u4;
		int nPad;
		int k;

		if (nLeft >= 5u) {
			nPad = 0;
		} else if (nLeft == 4u) {
			nPad = 1;
		} else if (nLeft == 3u) {
			nPad = 3;
		} else if (nLeft == 2u) {
			nPad = 4;
		} else {
			nPad = 6;
		}

		/*
		 * Emit high 5-bit index first (k = 7), down to low index
		 * (k = 0). The last nPad symbols are '='.
		 */
		for (k = 7; k >= 0; k--) {
			if (k < nPad) {
				out[iOut++] = '=';
			} else {
				out[iOut++] =
				    s_b296_b32[(u64 >> (unsigned)(k * 5)) & 31u];
			}
		}
	}

	out[iOut] = '\0';
	return (int)iOut;
}

int __gj_b32_encode(const uint8_t *in, size_t nin, char *out, size_t out_cap)
    __attribute__((alias("gj_b32_encode")));
