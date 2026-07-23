/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5754: latency ms/us from frames + sample rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_latency_ms_5754(uint32_t frames, uint32_t rate_hz);
 *     - frames * 1000 / rate_hz (integer). rate_hz==0 → 0. Saturates
 *       at UINT32_MAX via 64-bit intermediate product.
 *   uint32_t gj_adev_latency_us_5754(uint32_t frames, uint32_t rate_hz);
 *     - frames * 1000000 / rate_hz (integer). rate_hz==0 → 0.
 *   uint32_t gj_batch_id_5754(void);
 *   __ aliases
 *   __libcgj_batch5754_marker = "libcgj-batch5754"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5754 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5754_marker[] = "libcgj-batch5754";

#define B5754_BATCH_ID  5754u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5754_sat_u32(uint64_t u64V)
{
	if (u64V > (uint64_t)0xffffffffu) {
		return 0xffffffffu;
	}
	return (uint32_t)u64V;
}

static uint32_t
b5754_ms(uint32_t u32Frames, uint32_t u32Rate)
{
	uint64_t u64Num;

	if (u32Rate == 0u) {
		return 0u;
	}
	u64Num = (uint64_t)u32Frames * 1000ull;
	return b5754_sat_u32(u64Num / (uint64_t)u32Rate);
}

static uint32_t
b5754_us(uint32_t u32Frames, uint32_t u32Rate)
{
	uint64_t u64Num;

	if (u32Rate == 0u) {
		return 0u;
	}
	u64Num = (uint64_t)u32Frames * 1000000ull;
	return b5754_sat_u32(u64Num / (uint64_t)u32Rate);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_latency_ms_5754 - convert buffer frames at rate_hz to ms.
 */
uint32_t
gj_adev_latency_ms_5754(uint32_t frames, uint32_t rate_hz)
{
	(void)NULL;
	return b5754_ms(frames, rate_hz);
}

/*
 * gj_adev_latency_us_5754 - convert buffer frames at rate_hz to us.
 */
uint32_t
gj_adev_latency_us_5754(uint32_t frames, uint32_t rate_hz)
{
	return b5754_us(frames, rate_hz);
}

uint32_t
gj_batch_id_5754(void)
{
	return B5754_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_latency_ms_5754(uint32_t frames, uint32_t rate_hz)
    __attribute__((alias("gj_adev_latency_ms_5754")));

uint32_t __gj_adev_latency_us_5754(uint32_t frames, uint32_t rate_hz)
    __attribute__((alias("gj_adev_latency_us_5754")));

uint32_t __gj_batch_id_5754(void)
    __attribute__((alias("gj_batch_id_5754")));
