/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch286: COBS (Consistent Overhead Byte Stuffing)
 * decode — Cheshire & Baker / public framing algorithm, clean-room.
 *
 * Surface (unique symbols):
 *   int gj_cobs_decode(const uint8_t *in, size_t nin,
 *                      uint8_t *out, size_t out_cap, size_t *nout);
 *     — Decode classic COBS (no framing delimiter required in `in`).
 *       Writes decoded octets to out[0..*nout). Returns 0 on success,
 *       -1 on error. On success *nout is the decoded length; on error
 *       *nout is set to 0 when nout is non-NULL.
 *   __gj_cobs_decode  (alias)
 *   __libcgj_batch286_marker = "libcgj-batch286"
 *
 * COBS decode (public algorithm):
 *   While input remains:
 *     code = next byte; code == 0 is invalid.
 *     Copy the next (code - 1) data bytes (must be non-zero) to output.
 *     If code != 0xFF and more encoded input remains, emit a zero octet.
 *   Empty payload encodes as a single 0x01; empty `nin` yields empty out.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch286_marker[] = "libcgj-batch286";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cobs_decode — classic COBS decode into a bounded output buffer.
 *
 * in / nin:  encoded octets (no required trailing 0x00 frame delimiter).
 * out / out_cap: destination buffer and capacity in bytes.
 * nout: required; set to decoded length on success, 0 on error.
 *
 * Errors (-1):
 *   - nout == NULL
 *   - in == NULL && nin != 0
 *   - out == NULL && decoded length would be > 0 (empty decode may
 *     leave out unused when out_cap allows zero write)
 *   - truncated run (code claims more data bytes than remain)
 *   - zero octet inside a data run (invalid COBS)
 *   - output capacity exhausted
 *   - code byte == 0
 */
int
gj_cobs_decode(const uint8_t *in, size_t nin, uint8_t *out, size_t out_cap,
               size_t *nout)
{
	const uint8_t *pIn;
	uint8_t *pOut;
	size_t cbIn;
	size_t cbOut;
	size_t iIn;
	size_t iOut;

	if (nout != NULL) {
		*nout = 0u;
	}
	if (nout == NULL) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}
	/* Empty encoded stream → empty payload. */
	if (nin == 0u) {
		if (out == NULL && out_cap != 0u) {
			return -1;
		}
		*nout = 0u;
		return 0;
	}
	if (out == NULL && out_cap != 0u) {
		return -1;
	}

	pIn = in;
	pOut = out;
	cbIn = nin;
	cbOut = out_cap;
	iIn = 0u;
	iOut = 0u;

	while (iIn < cbIn) {
		uint8_t uCode;
		size_t cbData;
		size_t iCopy;

		uCode = pIn[iIn++];
		if (uCode == 0u) {
			*nout = 0u;
			return -1;
		}

		/* Data run length is code - 1 (0..254). */
		cbData = (size_t)uCode - 1u;
		if (cbData > cbIn - iIn) {
			*nout = 0u;
			return -1;
		}
		if (cbData > 0u) {
			if (pOut == NULL) {
				*nout = 0u;
				return -1;
			}
			if (cbData > cbOut - iOut) {
				*nout = 0u;
				return -1;
			}
			for (iCopy = 0u; iCopy < cbData; iCopy++) {
				uint8_t uByte = pIn[iIn + iCopy];

				/* Zeros are stuffed; must not appear in run. */
				if (uByte == 0u) {
					*nout = 0u;
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
				*nout = 0u;
				return -1;
			}
			if (iOut >= cbOut) {
				*nout = 0u;
				return -1;
			}
			pOut[iOut++] = 0u;
		}
	}

	*nout = iOut;
	return 0;
}

int __gj_cobs_decode(const uint8_t *in, size_t nin, uint8_t *out,
                     size_t out_cap, size_t *nout)
    __attribute__((alias("gj_cobs_decode")));
