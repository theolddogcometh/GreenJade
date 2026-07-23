/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch289: simple run-length decode (byte,count pairs).
 * Counterpart to batch288 gj_rle_compress.
 *
 * Surface (unique symbols):
 *   int gj_rle_decompress(const uint8_t *in, size_t nin,
 *                         uint8_t *out, size_t out_cap, size_t *nout);
 *     — Decode consecutive (byte, count) pairs from in[0..nin) into out.
 *       count is in 1..255; each pair expands to count copies of byte.
 *       *nout = total bytes written.
 *       Returns 0 on success, -1 on error (NULL args, odd nin, count==0,
 *       insufficient out_cap). On error *nout is 0 when nout is non-NULL.
 *       out is undefined on error.
 *   __gj_rle_decompress  (alias)
 *   __libcgj_batch289_marker = "libcgj-batch289"
 *
 * Empty input (nin == 0) writes nothing and returns 0 with *nout = 0.
 * in may be NULL only when nin == 0.
 * Format matches batch288: stream is always even length (2 * pair_count).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch289_marker[] = "libcgj-batch289";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_decompress — expand simple RLE (value, run_len) pairs.
 *
 * Walk in left-to-right two bytes at a time. For each (val, cnt) with
 * cnt in 1..255, emit cnt copies of val into out. Matches batch288
 * gj_rle_compress output exactly.
 */
int
gj_rle_decompress(const uint8_t *in, size_t nin, uint8_t *out, size_t out_cap,
                  size_t *nout)
{
	size_t iIn;
	size_t iOut;
	uint8_t u8Val;
	uint8_t u8Cnt;
	size_t nRun;
	size_t i;

	if (nout != NULL) {
		*nout = 0u;
	}
	if (out == NULL || nout == NULL) {
		return -1;
	}
	if (in == NULL && nin != 0u) {
		return -1;
	}
	if (nin == 0u) {
		return 0;
	}
	/* Stream is packed as 2-byte pairs; odd length is malformed. */
	if ((nin & 1u) != 0u) {
		return -1;
	}

	iIn = 0u;
	iOut = 0u;

	while (iIn < nin) {
		u8Val = in[iIn];
		u8Cnt = in[iIn + 1u];
		iIn += 2u;

		/* Compress never emits zero-length runs. */
		if (u8Cnt == 0u) {
			*nout = 0u;
			return -1;
		}

		nRun = (size_t)u8Cnt;
		if (iOut > out_cap || nRun > out_cap - iOut) {
			*nout = 0u;
			return -1;
		}
		for (i = 0u; i < nRun; i++) {
			out[iOut + i] = u8Val;
		}
		iOut += nRun;
	}

	*nout = iOut;
	return 0;
}

int __gj_rle_decompress(const uint8_t *in, size_t nin, uint8_t *out,
                        size_t out_cap, size_t *nout)
    __attribute__((alias("gj_rle_decompress")));
