/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5757: period/buffer frame stubs from rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_period_frames_5757(uint32_t rate_hz,
 *                                       uint32_t period_ms);
 *     - rate_hz * period_ms / 1000 (half-up). rate or period 0 → 0.
 *       Result clamped to at least 1 when both inputs non-zero and
 *       product underflows to 0 (sub-millisecond rates).
 *   uint32_t gj_adev_buffer_frames_5757(uint32_t period_frames,
 *                                       uint32_t n_periods);
 *     - period_frames * n_periods with saturating u32 multiply.
 *       n_periods clamped to [1, 64] when period_frames != 0; if
 *       period_frames==0 → 0.
 *   uint32_t gj_adev_period_count_5757(uint32_t buffer_frames,
 *                                      uint32_t period_frames);
 *     - buffer_frames / period_frames (integer). period_frames==0 → 0.
 *   uint32_t gj_batch_id_5757(void);
 *   __ aliases
 *   __libcgj_batch5757_marker = "libcgj-batch5757"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5757 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5757_marker[] = "libcgj-batch5757";

#define B5757_BATCH_ID   5757u
#define B5757_NPER_MAX   64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5757_sat_u32(uint64_t u64V)
{
	if (u64V > (uint64_t)0xffffffffu) {
		return 0xffffffffu;
	}
	return (uint32_t)u64V;
}

static uint32_t
b5757_period(uint32_t u32Rate, uint32_t u32Ms)
{
	uint64_t u64Num;
	uint32_t u32R;

	if (u32Rate == 0u || u32Ms == 0u) {
		return 0u;
	}
	u64Num = (uint64_t)u32Rate * (uint64_t)u32Ms + 500ull;
	u32R = b5757_sat_u32(u64Num / 1000ull);
	if (u32R == 0u) {
		return 1u;
	}
	return u32R;
}

static uint32_t
b5757_buffer(uint32_t u32Period, uint32_t u32N)
{
	if (u32Period == 0u) {
		return 0u;
	}
	if (u32N == 0u) {
		u32N = 1u;
	}
	if (u32N > B5757_NPER_MAX) {
		u32N = B5757_NPER_MAX;
	}
	return b5757_sat_u32((uint64_t)u32Period * (uint64_t)u32N);
}

static uint32_t
b5757_count(uint32_t u32Buf, uint32_t u32Period)
{
	if (u32Period == 0u) {
		return 0u;
	}
	return u32Buf / u32Period;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_period_frames_5757 - period length in frames from rate + ms.
 */
uint32_t
gj_adev_period_frames_5757(uint32_t rate_hz, uint32_t period_ms)
{
	(void)NULL;
	return b5757_period(rate_hz, period_ms);
}

/*
 * gj_adev_buffer_frames_5757 - total buffer = period * n_periods.
 */
uint32_t
gj_adev_buffer_frames_5757(uint32_t period_frames, uint32_t n_periods)
{
	return b5757_buffer(period_frames, n_periods);
}

/*
 * gj_adev_period_count_5757 - how many periods fit in buffer.
 */
uint32_t
gj_adev_period_count_5757(uint32_t buffer_frames, uint32_t period_frames)
{
	return b5757_count(buffer_frames, period_frames);
}

uint32_t
gj_batch_id_5757(void)
{
	return B5757_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_period_frames_5757(uint32_t rate_hz, uint32_t period_ms)
    __attribute__((alias("gj_adev_period_frames_5757")));

uint32_t __gj_adev_buffer_frames_5757(uint32_t period_frames,
                                      uint32_t n_periods)
    __attribute__((alias("gj_adev_buffer_frames_5757")));

uint32_t __gj_adev_period_count_5757(uint32_t buffer_frames,
                                     uint32_t period_frames)
    __attribute__((alias("gj_adev_period_count_5757")));

uint32_t __gj_batch_id_5757(void)
    __attribute__((alias("gj_batch_id_5757")));
