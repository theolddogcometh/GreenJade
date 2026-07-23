/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1503: simple run-length decode (byte,count).
 * Counterpart to batch1502 gj_rle_encode_simple.
 *
 * Surface (unique symbols):
 *   int gj_rle_decode_simple(const uint8_t *in, size_t nin,
 *                            uint8_t *out, size_t out_cap, size_t *nout);
 *     — Decode consecutive (byte, count) pairs from in[0..nin) into out.
 *       count is in 1..255; each pair expands to count copies of byte.
 *       *nout = total bytes written.
 *       Returns 0 on success, -1 on error (NULL args, odd nin, count==0,
 *       insufficient out_cap). On error *nout is 0 when nout is non-NULL.
 *       out is undefined on error.
 *   int __gj_rle_decode_simple  (alias)
 *   __libcgj_batch1503_marker = "libcgj-batch1503"
 *
 * Empty input (nin == 0) writes nothing and returns 0 with *nout = 0.
 * in may be NULL only when nin == 0.
 * Format matches batch1502: stream is always even length (2 * pair_count).
 * Distinct from gj_rle_decompress (batch289) — unique symbols only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1503_marker[] = "libcgj-batch1503";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_decode_simple — expand simple RLE (value, run_len) pairs.
 *
 * Walk in left-to-right two bytes at a time. For each (val, cnt) with
 * cnt in 1..255, emit cnt copies of val into out. Matches batch1502
 * gj_rle_encode_simple output exactly.
 */
int
gj_rle_decode_simple(const uint8_t *pIn, size_t cbIn, uint8_t *pOut,
    size_t cbOutCap, size_t *pcbOut)
{
	size_t iIn;
	size_t iOut;
	uint8_t u8Val;
	uint8_t u8Cnt;
	size_t nRun;
	size_t i;

	if (pcbOut != NULL) {
		*pcbOut = 0u;
	}
	if (pOut == NULL || pcbOut == NULL) {
		return -1;
	}
	if (pIn == NULL && cbIn != 0u) {
		return -1;
	}
	if (cbIn == 0u) {
		return 0;
	}
	/* Stream is packed as 2-byte pairs; odd length is malformed. */
	if ((cbIn & 1u) != 0u) {
		return -1;
	}

	iIn = 0u;
	iOut = 0u;

	while (iIn < cbIn) {
		u8Val = pIn[iIn];
		u8Cnt = pIn[iIn + 1u];
		iIn += 2u;

		/* Encode never emits zero-length runs. */
		if (u8Cnt == 0u) {
			*pcbOut = 0u;
			return -1;
		}

		nRun = (size_t)u8Cnt;
		if (iOut > cbOutCap || nRun > cbOutCap - iOut) {
			*pcbOut = 0u;
			return -1;
		}
		for (i = 0u; i < nRun; i++) {
			pOut[iOut + i] = u8Val;
		}
		iOut += nRun;
	}

	*pcbOut = iOut;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rle_decode_simple(const uint8_t *pIn, size_t cbIn, uint8_t *pOut,
    size_t cbOutCap, size_t *pcbOut)
    __attribute__((alias("gj_rle_decode_simple")));
