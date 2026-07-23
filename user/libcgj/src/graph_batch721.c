/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch721: COBS encode (Consistent Overhead Byte Stuffing).
 *
 * Surface (unique symbols):
 *   int gj_cobs_encode_len(const unsigned char *in, size_t n,
 *                      unsigned char *out, size_t cap);
 *     — Encode n bytes at in into classic COBS form at out[0..cap).
 *       Returns encoded length on success, or -1 on error.
 *       Empty (n == 0) encodes to a single 0x01 code byte.
 *       in may be NULL only when n == 0; out must be non-NULL.
 *       No trailing frame delimiter (0x00) is appended — pure COBS payload.
 *   __gj_cobs_encode_len_len  (alias)
 *   __libcgj_batch721_marker = "libcgj-batch721"
 *
 * Algorithm (Cheshire / Baker-Harvey COBS, clean-room):
 *   Walk input; non-zero runs are emitted under a code byte whose value
 *   is (run_len + 1). A zero in the input finishes the current code and
 *   starts a new one (implied zero, not stored). Code 0xFF finishes a
 *   full 254-byte non-zero block with no implied zero after it.
 *
 *   Worst-case output size: n + 1 + floor(n / 254).
 *   Empty input → 1 byte (0x01).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch721_marker[] = "libcgj-batch721";

/* Max non-zero data bytes under one COBS code (code value 0xFF). */
#define B721_MAX_RUN 254u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cobs_encode_len — Consistent Overhead Byte Stuffing encoder.
 *
 * in:  input bytes (NULL only legal when n == 0)
 * n:   input length in bytes
 * out: output buffer (must be non-NULL)
 * cap: capacity of out in bytes
 *
 * Returns the number of bytes written on success, or -1 on NULL args,
 * NULL in with n > 0, or when cap is smaller than the encoded size.
 *
 * Framing: does not write a trailing 0x00 packet delimiter; callers that
 * need zero-delimited frames append it themselves after a successful encode.
 *
 * Note: return type is int; encoded lengths that do not fit in a positive
 * int are rejected as -1 (pathological for freestanding buffers).
 */
int
gj_cobs_encode_len(const unsigned char *in, size_t n, unsigned char *out,
    size_t cap)
{
	const unsigned char *pIn;
	size_t iIn;
	size_t iOut;
	size_t iCode;
	unsigned char uCode;

	if (out == NULL) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	/*
	 * Need at least one byte for the first code placeholder. Empty
	 * input finishes immediately as a single 0x01.
	 */
	if (cap < 1u) {
		return -1;
	}

	pIn = in;
	iCode = 0u;
	out[iCode] = 0u; /* placeholder; filled when the run closes */
	iOut = 1u;
	uCode = 1u;

	for (iIn = 0u; iIn < n; iIn++) {
		unsigned char uByte = pIn[iIn];

		if (uByte == 0u) {
			/* Close current code (implied zero; not stored). */
			out[iCode] = uCode;
			if (iOut >= cap) {
				return -1;
			}
			iCode = iOut;
			out[iCode] = 0u;
			iOut++;
			uCode = 1u;
		} else {
			if (iOut >= cap) {
				return -1;
			}
			out[iOut] = uByte;
			iOut++;
			uCode++;
			if (uCode == (unsigned char)(B721_MAX_RUN + 1u)) {
				/*
				 * Full 254-byte non-zero block: code 0xFF,
				 * no implied zero; open a fresh code.
				 */
				out[iCode] = uCode;
				if (iOut >= cap) {
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

	/* Encoded length must fit a positive int return. */
	if (iOut > (size_t)0x7fffffff) {
		return -1;
	}
	return (int)iOut;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cobs_encode_len_len(const unsigned char *in, size_t n, unsigned char *out,
    size_t cap)
    __attribute__((alias("gj_cobs_encode_len")));
