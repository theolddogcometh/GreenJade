/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1677: rough fixed-point RMS of s16 PCM (milli-FS).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_rms_s16_milli(const int16_t *samples, size_t n);
 *     — Rough RMS = isqrt(sum(s*s) / n), then scaled to milli of full
 *       scale: rms * 1000 / 32768. Result roughly in [0, 1000] for
 *       in-range s16 (slightly above 1000 only if |s|==32768 path via
 *       squares of -32768). NULL or n==0 → 0.
 *   uint32_t __gj_pcm_rms_s16_milli  (alias)
 *   __libcgj_batch1677_marker = "libcgj-batch1677"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1677_marker[] = "libcgj-batch1677";

#define B1677_S16_FS  32768u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Integer square root of u64X via binary digit-by-digit method.
 * Floor(sqrt(x)); pure, no float.
 */
static uint32_t
b1677_isqrt_u64(uint64_t u64X)
{
	uint64_t u64Res;
	uint64_t u64Bit;

	u64Res = 0ull;
	/* Highest power of 4 <= 2^62 for 64-bit input. */
	u64Bit = 1ull << 62;

	while (u64Bit > u64X) {
		u64Bit >>= 2;
	}

	while (u64Bit != 0ull) {
		if (u64X >= u64Res + u64Bit) {
			u64X -= u64Res + u64Bit;
			u64Res = (u64Res >> 1) + u64Bit;
		} else {
			u64Res >>= 1;
		}
		u64Bit >>= 2;
	}
	return (uint32_t)u64Res;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_rms_s16_milli — rough fixed RMS of an s16 buffer in milli-FS.
 *
 * Mean of squares uses integer division (trunc toward zero). isqrt then
 * scales by 1000/32768 so  full-scale constant tone ≈ 1000 milli.
 */
uint32_t
gj_pcm_rms_s16_milli(const int16_t *pSamples, size_t cN)
{
	uint64_t u64SumSq;
	uint64_t u64MeanSq;
	uint32_t u32Rms;
	uint64_t u64Milli;
	size_t iSample;

	if (pSamples == NULL || cN == 0u) {
		return 0u;
	}

	u64SumSq = 0ull;
	for (iSample = 0u; iSample < cN; iSample++) {
		int32_t i32S = (int32_t)pSamples[iSample];
		u64SumSq += (uint64_t)((int64_t)i32S * (int64_t)i32S);
	}

	u64MeanSq = u64SumSq / (uint64_t)cN;
	u32Rms = b1677_isqrt_u64(u64MeanSq);

	/* milli-FS = rms * 1000 / 32768 */
	u64Milli = ((uint64_t)u32Rms * 1000ull) / (uint64_t)B1677_S16_FS;
	if (u64Milli > 0xffffffffull) {
		return 0xffffffffu;
	}
	return (uint32_t)u64Milli;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_rms_s16_milli(const int16_t *pSamples, size_t cN)
    __attribute__((alias("gj_pcm_rms_s16_milli")));
