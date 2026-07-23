/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2563: simple run-length encode (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_rle_encode_u(const uint8_t *in, size_t nin,
 *                       uint8_t *out, size_t out_cap, size_t *nout);
 *     — Encode in[0..nin) as consecutive (byte, count) pairs into out.
 *       count is in 1..255; runs longer than 255 are split across pairs.
 *       *nout = total bytes written (always even: 2 * pair_count).
 *       Returns 0 on success, -1 on error (NULL args, insufficient out_cap).
 *       On error *nout is 0 when nout is non-NULL. out is undefined on error.
 *   int __gj_rle_encode_u  (alias)
 *   __libcgj_batch2563_marker = "libcgj-batch2563"
 *
 * Empty input (nin == 0) writes nothing and returns 0 with *nout = 0.
 * in may be NULL only when nin == 0.
 * Bound via gj_rle_bound_u (batch2562); decode via batch2564.
 * Distinct from gj_rle_compress (batch288) / gj_rle_encode_simple
 * (batch1502) — unique symbols only.
 *
 * Compression exclusive wave (2561–2570). Clean-room freestanding pure C
 * (integer/pointer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2563_marker[] = "libcgj-batch2563";

/* Max run length storable in one count byte. */
#define B2563_RUN_MAX 255u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Emit (value, run_len) pairs for in[0..cbIn) into pOut.
 * Returns 0 on success, -1 on error. *pcbOut set on all paths when non-NULL.
 */
static int
b2563_encode(const uint8_t *pIn, size_t cbIn, uint8_t *pOut,
    size_t cbOutCap, size_t *pcbOut)
{
	size_t iIn;
	size_t iOut;
	uint8_t u8Val;
	size_t nRun;

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

	iIn = 0u;
	iOut = 0u;

	while (iIn < cbIn) {
		u8Val = pIn[iIn];
		nRun = 1u;
		while (iIn + nRun < cbIn && pIn[iIn + nRun] == u8Val &&
		    nRun < (size_t)B2563_RUN_MAX) {
			nRun++;
		}
		/* nRun is in 1..255 here (capped by B2563_RUN_MAX). */
		if (iOut + 2u > cbOutCap) {
			*pcbOut = 0u;
			return -1;
		}
		pOut[iOut] = u8Val;
		pOut[iOut + 1u] = (uint8_t)nRun;
		iOut += 2u;
		iIn += nRun;
	}

	*pcbOut = iOut;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rle_encode_u — emit (value, run_len) for each run chunk.
 *
 * Walk in left-to-right. A run is a maximal contiguous sequence of the
 * same byte, chunked at B2563_RUN_MAX so the count always fits in one
 * uint8_t. Each chunk consumes exactly two output bytes.
 */
int
gj_rle_encode_u(const uint8_t *in, size_t nin, uint8_t *out, size_t out_cap,
    size_t *nout)
{
	(void)NULL;
	return b2563_encode(in, nin, out, out_cap, nout);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rle_encode_u(const uint8_t *in, size_t nin, uint8_t *out,
    size_t out_cap, size_t *nout)
    __attribute__((alias("gj_rle_encode_u")));
