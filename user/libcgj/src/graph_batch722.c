/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch722: COBS (Consistent Overhead Byte Stuffing)
 * decode — Cheshire / Baker-Harvey public algorithm, clean-room.
 *
 * Surface (unique symbols):
 *   int gj_cobs_decode_ok(const unsigned char *in, size_t n,
 *                      unsigned char *out, size_t cap);
 *     — Decode classic COBS (no framing delimiter required in `in`).
 *       Writes decoded octets to out[0..). Returns 0 (OK) on success,
 *       -1 (FAIL) on error. Capacity must hold the full decoded payload.
 *   int __gj_cobs_decode_ok_ok  (alias)
 *   __libcgj_batch722_marker = "libcgj-batch722"
 *
 * COBS decode (public algorithm):
 *   While input remains:
 *     code = next byte; code == 0 is invalid.
 *     Copy the next (code - 1) data bytes (must be non-zero) to output.
 *     If code != 0xFF and more encoded input remains, emit a zero octet.
 *   Empty payload encodes as a single 0x01; empty `n` yields empty out.
 *
 * Note: batch286 exposes a 5-arg form with size_t *nout. This TU is the
 * 4-arg OK/FAIL surface only — do not link both if symbols collide.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch722_marker[] = "libcgj-batch722";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cobs_decode_ok — classic COBS decode into a bounded output buffer.
 *
 * in / n:   encoded octets (no required trailing 0x00 frame delimiter).
 * out / cap: destination buffer and capacity in bytes.
 *
 * Returns 0 (OK) on success, -1 (FAIL) on:
 *   - in == NULL && n != 0
 *   - out == NULL when a non-empty write would occur, or when cap != 0
 *     and out is NULL (null out only legal for zero-capacity empty decode)
 *   - truncated run (code claims more data bytes than remain)
 *   - zero octet inside a data run (invalid COBS)
 *   - output capacity exhausted
 *   - code byte == 0
 *
 * Empty encoded stream (n == 0) → empty payload, returns 0.
 */
int
gj_cobs_decode_ok(const unsigned char *in, size_t n, unsigned char *out,
               size_t cap)
{
	const unsigned char *pIn;
	unsigned char *pOut;
	size_t cbIn;
	size_t cbOut;
	size_t iIn;
	size_t iOut;

	if (in == NULL && n != 0u) {
		return -1;
	}
	/* Empty encoded stream → empty payload. */
	if (n == 0u) {
		if (out == NULL && cap != 0u) {
			return -1;
		}
		return 0;
	}
	if (out == NULL && cap != 0u) {
		return -1;
	}

	pIn = in;
	pOut = out;
	cbIn = n;
	cbOut = cap;
	iIn = 0u;
	iOut = 0u;

	while (iIn < cbIn) {
		unsigned char uCode;
		size_t cbData;
		size_t iCopy;

		uCode = pIn[iIn++];
		if (uCode == 0u) {
			return -1;
		}

		/* Data run length is code - 1 (0..254). */
		cbData = (size_t)uCode - 1u;
		if (cbData > cbIn - iIn) {
			return -1;
		}
		if (cbData > 0u) {
			if (pOut == NULL) {
				return -1;
			}
			if (cbData > cbOut - iOut) {
				return -1;
			}
			for (iCopy = 0u; iCopy < cbData; iCopy++) {
				unsigned char uByte = pIn[iIn + iCopy];

				/* Zeros are stuffed; must not appear in run. */
				if (uByte == 0u) {
					return -1;
				}
				pOut[iOut + iCopy] = uByte;
			}
			iIn += cbData;
			iOut += cbData;
		}

		/*
		 * code != 0xFF means a zero was removed after this run
		 * (or the run was short). Emit that zero only when more
		 * encoded input remains (not after the final group).
		 */
		if (uCode != 0xffu && iIn < cbIn) {
			if (pOut == NULL) {
				return -1;
			}
			if (iOut >= cbOut) {
				return -1;
			}
			pOut[iOut++] = 0u;
		}
	}

	(void)iOut;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cobs_decode_ok_ok(const unsigned char *in, size_t n, unsigned char *out,
                     size_t cap)
    __attribute__((alias("gj_cobs_decode_ok")));
