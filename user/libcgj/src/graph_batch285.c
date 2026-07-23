/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch285: COBS encode (Consistent Overhead Byte Stuffing).
 *
 * Surface (unique symbols):
 *   int gj_cobs_encode(const uint8_t *in, size_t nin, uint8_t *out,
 *                      size_t out_cap, size_t *nout);
 *     — Encode nin bytes at in into COBS form at out[0..out_cap).
 *       *nout = encoded length on success. Returns 0 / -1.
 *       Empty (nin == 0) encodes to a single 0x01 code byte.
 *       in may be NULL only when nin == 0; out and nout must be non-NULL.
 *       No trailing frame delimiter (0x00) is appended — pure COBS payload.
 *   __gj_cobs_encode  (alias)
 *   __libcgj_batch285_marker = "libcgj-batch285"
 *
 * Algorithm (Cheshire / Baker-Harvey COBS, clean-room):
 *   Walk input; non-zero runs are emitted under a code byte whose value
 *   is (run_len + 1). A zero in the input finishes the current code and
 *   starts a new one (implicit zero, not stored). Code 0xFF finishes a
 *   full 254-byte non-zero block with no implied zero after it.
 *
 *   Worst-case output size: nin + 1 + floor(nin / 254).
 *   Empty input → 1 byte (0x01).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch285_marker[] = "libcgj-batch285";

/* Max non-zero data bytes under one COBS code (code value 0xFF). */
#define B285_MAX_RUN 254u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cobs_encode — Consistent Overhead Byte Stuffing encoder.
 *
 * in:      input bytes (NULL only legal when nin == 0)
 * nin:     input length in bytes
 * out:     output buffer (must be non-NULL)
 * out_cap: capacity of out in bytes
 * nout:    on success, set to number of bytes written (must be non-NULL)
 *
 * Returns 0 on success, -1 on NULL args, NULL in with nin > 0, or when
 * out_cap is smaller than the encoded size.
 *
 * Framing: does not write a trailing 0x00 packet delimiter; callers that
 * need zero-delimited frames append it themselves after a successful encode.
 */
int
gj_cobs_encode(const uint8_t *in, size_t nin, uint8_t *out, size_t out_cap,
               size_t *nout)
{
	const uint8_t *pIn;
	size_t iIn;
	size_t iOut;
	size_t iCode;
	uint8_t uCode;

	if (out == NULL || nout == NULL) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}

	/*
	 * Need at least one byte for the first code placeholder. Empty
	 * input finishes immediately as a single 0x01.
	 */
	if (out_cap < 1u) {
		return -1;
	}

	pIn = in;
	iCode = 0u;
	out[iCode] = 0u; /* placeholder; filled when the run closes */
	iOut = 1u;
	uCode = 1u;

	for (iIn = 0u; iIn < nin; iIn++) {
		uint8_t uByte = pIn[iIn];

		if (uByte == 0u) {
			/* Close current code (implied zero; not stored). */
			out[iCode] = uCode;
			if (iOut >= out_cap) {
				return -1;
			}
			iCode = iOut;
			out[iCode] = 0u;
			iOut++;
			uCode = 1u;
		} else {
			if (iOut >= out_cap) {
				return -1;
			}
			out[iOut] = uByte;
			iOut++;
			uCode++;
			if (uCode == (uint8_t)(B285_MAX_RUN + 1u)) {
				/*
				 * Full 254-byte non-zero block: code 0xFF,
				 * no implied zero; open a fresh code.
				 */
				out[iCode] = uCode;
				if (iOut >= out_cap) {
					return -1;
				}
				iCode = iOut;
				out[iCode] = 0u;
				iOut++;
				uCode = 1u;
			}
		}
	}

	/* Finish final code. */
	out[iCode] = uCode;
	*nout = iOut;
	return 0;
}

int __gj_cobs_encode(const uint8_t *in, size_t nin, uint8_t *out,
                     size_t out_cap, size_t *nout)
    __attribute__((alias("gj_cobs_encode")));
