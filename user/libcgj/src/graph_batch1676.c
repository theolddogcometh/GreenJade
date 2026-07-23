/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1676: interleave mono L/R s16 streams to stereo.
 *
 * Surface (unique symbols):
 *   size_t gj_pcm_stereo_interleave_s16(const int16_t *left,
 *                                       const int16_t *right,
 *                                       int16_t *dst, size_t n);
 *     — For i in [0, n): dst[2*i] = left[i], dst[2*i+1] = right[i].
 *       Returns frames written (n). NULL left/right/dst or n==0 → 0.
 *       Caller must ensure dst holds at least 2*n int16 slots.
 *   size_t __gj_pcm_stereo_interleave_s16  (alias)
 *   __libcgj_batch1676_marker = "libcgj-batch1676"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1676_marker[] = "libcgj-batch1676";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_stereo_interleave_s16 — pack planar mono L/R into interleaved stereo.
 *
 * Output layout: L0, R0, L1, R1, ... (little-endian host stores as-is).
 * Returns the number of stereo frames written.
 */
size_t
gj_pcm_stereo_interleave_s16(const int16_t *pLeft, const int16_t *pRight,
    int16_t *pDst, size_t cN)
{
	size_t iFrame;

	if (pLeft == NULL || pRight == NULL || pDst == NULL || cN == 0u) {
		return 0u;
	}

	for (iFrame = 0u; iFrame < cN; iFrame++) {
		pDst[2u * iFrame] = pLeft[iFrame];
		pDst[2u * iFrame + 1u] = pRight[iFrame];
	}
	return cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_pcm_stereo_interleave_s16(const int16_t *pLeft,
    const int16_t *pRight, int16_t *pDst, size_t cN)
    __attribute__((alias("gj_pcm_stereo_interleave_s16")));
