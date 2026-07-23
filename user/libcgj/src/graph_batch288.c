/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch288: simple run-length encode (byte,count pairs).
 *
 * Surface (unique symbols):
 *   int gj_rle_compress(const uint8_t *in, size_t nin,
 *                       uint8_t *out, size_t out_cap, size_t *nout);
 *     — Encode in[0..nin) as consecutive (byte, count) pairs into out.
 *       count is in 1..255; runs longer than 255 are split across pairs.
 *       *nout = total bytes written (always even: 2 * pair_count).
 *       Returns 0 on success, -1 on error (NULL args, insufficient out_cap).
 *       On error *nout is 0 when nout is non-NULL. out is undefined on error.
 *   __gj_rle_compress  (alias)
 *   __libcgj_batch288_marker = "libcgj-batch288"
 *
 * Empty input (nin == 0) writes nothing and returns 0 with *nout = 0.
 * in may be NULL only when nin == 0.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch288_marker[] = "libcgj-batch288";

/* Max run length storable in one count byte. */
#define B288_RUN_MAX 255u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_compress — simple RLE: emit (value, run_len) for each run.
 *
 * Walk in left-to-right. A run is a maximal contiguous sequence of the
 * same byte, chunked at B288_RUN_MAX so the count always fits in one
 * uint8_t. Each chunk consumes exactly two output bytes.
 */
int
gj_rle_compress(const uint8_t *in, size_t nin, uint8_t *out, size_t out_cap,
                size_t *nout)
{
	size_t iIn;
	size_t iOut;
	uint8_t u8Val;
	size_t nRun;

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

	iIn = 0u;
	iOut = 0u;

	while (iIn < nin) {
		u8Val = in[iIn];
		nRun = 1u;
		while (iIn + nRun < nin && in[iIn + nRun] == u8Val &&
		       nRun < (size_t)B288_RUN_MAX) {
			nRun++;
		}
		/* nRun is in 1..255 here (capped by B288_RUN_MAX). */
		if (iOut + 2u > out_cap) {
			*nout = 0u;
			return -1;
		}
		out[iOut] = u8Val;
		out[iOut + 1u] = (uint8_t)nRun;
		iOut += 2u;
		iIn += nRun;
	}

	*nout = iOut;
	return 0;
}

int __gj_rle_compress(const uint8_t *in, size_t nin, uint8_t *out,
                      size_t out_cap, size_t *nout)
    __attribute__((alias("gj_rle_compress")));
