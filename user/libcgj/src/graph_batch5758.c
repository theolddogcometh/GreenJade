/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5758: sink/source rate ratio (Q16) + match.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_rate_ratio_q16_5758(uint32_t src_hz, uint32_t dst_hz);
 *     - (src_hz << 16) / dst_hz as Q16.16 fixed. src or dst 0 → 0.
 *       Saturates at UINT32_MAX when product overflows 64-bit divide
 *       result (rare for audio rates).
 *   int gj_adev_rate_match_5758(uint32_t a_hz, uint32_t b_hz);
 *     - 1 if a_hz == b_hz and both non-zero, else 0.
 *   uint32_t gj_adev_rate_gcd_5758(uint32_t a_hz, uint32_t b_hz);
 *     - Euclidean GCD of two rates (for simple integer resamplers).
 *       Either 0 → 0.
 *   uint32_t gj_batch_id_5758(void);
 *   __ aliases
 *   __libcgj_batch5758_marker = "libcgj-batch5758"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5758 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5758_marker[] = "libcgj-batch5758";

#define B5758_BATCH_ID  5758u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5758_sat_u32(uint64_t u64V)
{
	if (u64V > (uint64_t)0xffffffffu) {
		return 0xffffffffu;
	}
	return (uint32_t)u64V;
}

static uint32_t
b5758_ratio_q16(uint32_t u32Src, uint32_t u32Dst)
{
	uint64_t u64Num;

	if (u32Src == 0u || u32Dst == 0u) {
		return 0u;
	}
	u64Num = ((uint64_t)u32Src) << 16;
	return b5758_sat_u32(u64Num / (uint64_t)u32Dst);
}

static uint32_t
b5758_gcd(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32T;

	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	while (u32B != 0u) {
		u32T = u32A % u32B;
		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_rate_ratio_q16_5758 - Q16.16 src/dst rate ratio for resampler
 * stubs. Example: 48000/44100 ≈ 1.0884 → 0x0001172b-ish.
 */
uint32_t
gj_adev_rate_ratio_q16_5758(uint32_t src_hz, uint32_t dst_hz)
{
	(void)NULL;
	return b5758_ratio_q16(src_hz, dst_hz);
}

/*
 * gj_adev_rate_match_5758 - exact non-zero rate equality.
 */
int
gj_adev_rate_match_5758(uint32_t a_hz, uint32_t b_hz)
{
	if (a_hz == 0u || b_hz == 0u) {
		return 0;
	}
	return (a_hz == b_hz) ? 1 : 0;
}

/*
 * gj_adev_rate_gcd_5758 - GCD of two sample rates.
 */
uint32_t
gj_adev_rate_gcd_5758(uint32_t a_hz, uint32_t b_hz)
{
	return b5758_gcd(a_hz, b_hz);
}

uint32_t
gj_batch_id_5758(void)
{
	return B5758_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_rate_ratio_q16_5758(uint32_t src_hz, uint32_t dst_hz)
    __attribute__((alias("gj_adev_rate_ratio_q16_5758")));

int __gj_adev_rate_match_5758(uint32_t a_hz, uint32_t b_hz)
    __attribute__((alias("gj_adev_rate_match_5758")));

uint32_t __gj_adev_rate_gcd_5758(uint32_t a_hz, uint32_t b_hz)
    __attribute__((alias("gj_adev_rate_gcd_5758")));

uint32_t __gj_batch_id_5758(void)
    __attribute__((alias("gj_batch_id_5758")));
