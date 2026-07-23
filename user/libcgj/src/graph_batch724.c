/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch724: SLIP (RFC 1055) packet decode.
 *
 * Surface (unique symbols):
 *   int gj_slip_decode(const unsigned char *in, size_t n,
 *                      unsigned char *out, size_t cap);
 *     — Decode one SLIP frame from in[0..n) into out[0..cap).
 *       Strips leading END delimiters, unescapes stuffed octets, and
 *       stops at the first trailing END (ENDs are not written to out).
 *       Frame form (counterpart to batch723 gj_slip_encode):
 *         END* || stuffed || END
 *       Unstuffing (RFC 1055):
 *         ESC ESC_END → END (0xC0)
 *         ESC ESC_ESC → ESC (0xDB)
 *       Returns 0 (OK) on success, -1 (FAIL) on error.
 *   int __gj_slip_decode  (alias)
 *   __libcgj_batch724_marker = "libcgj-batch724"
 *
 * Distinct from gj_slip_encode (batch723), gj_cobs_decode (batch286 /
 * batch722), and SLIP-0039 RS1024 helpers (batch300). Unique
 * gj_slip_decode names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch724_marker[] = "libcgj-batch724";

/* RFC 1055 SLIP special octets. */
#define B724_END     0xC0u
#define B724_ESC     0xDBu
#define B724_ESC_END 0xDCu
#define B724_ESC_ESC 0xDDu

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slip_decode — RFC 1055 SLIP one-frame decode; strip ENDs.
 *
 * in:  encoded octets (NULL only legal when n == 0 — still FAIL, no frame)
 * n:   input length in bytes
 * out: destination buffer (must be non-NULL)
 * cap: capacity of out in bytes
 *
 * Skips any leading END bytes, then unstuffs until the next END. That
 * trailing END is consumed and not emitted. Only the first complete
 * frame is decoded; any octets after its trailing END are ignored.
 *
 * Edge cases / FAIL (-1):
 *   out == NULL
 *   in == NULL && n != 0
 *   no terminating END after optional leading ENDs (incomplete frame)
 *   ESC with no following octet, or ESC not followed by ESC_END/ESC_ESC
 *   decoded payload longer than cap
 *
 * Empty frame (e.g. END END after strip) writes nothing and returns 0
 * when out is non-NULL (cap may be 0).
 */
int
gj_slip_decode(const unsigned char *in, size_t n, unsigned char *out,
               size_t cap)
{
	size_t iIn;
	size_t iOut;
	int fSawEnd;

	if (out == NULL) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}
	if (n == 0u) {
		/* No octets → no complete frame. */
		return -1;
	}

	/* Strip leading ENDs (inter-frame / flush noise). */
	iIn = 0u;
	while (iIn < n && in[iIn] == (unsigned char)B724_END) {
		iIn++;
	}

	iOut = 0u;
	fSawEnd = 0;

	while (iIn < n) {
		unsigned char uByte = in[iIn++];

		if (uByte == (unsigned char)B724_END) {
			/* Trailing END: frame complete; strip, do not emit. */
			fSawEnd = 1;
			break;
		}

		if (uByte == (unsigned char)B724_ESC) {
			unsigned char uEsc;

			if (iIn >= n) {
				return -1;
			}
			uEsc = in[iIn++];
			if (uEsc == (unsigned char)B724_ESC_END) {
				uByte = (unsigned char)B724_END;
			} else if (uEsc == (unsigned char)B724_ESC_ESC) {
				uByte = (unsigned char)B724_ESC;
			} else {
				/* Protocol error: unknown escape. */
				return -1;
			}
		}

		if (iOut >= cap) {
			return -1;
		}
		out[iOut++] = uByte;
	}

	if (!fSawEnd) {
		return -1;
	}

	(void)iOut;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slip_decode(const unsigned char *in, size_t n, unsigned char *out,
                     size_t cap)
    __attribute__((alias("gj_slip_decode")));
