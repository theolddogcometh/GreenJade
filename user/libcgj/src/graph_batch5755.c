/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5755: frames from latency ms/us + sample rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_frames_from_ms_5755(uint32_t latency_ms,
 *                                        uint32_t rate_hz);
 *     - rate_hz * latency_ms / 1000 (integer, round half-up via +500).
 *       rate_hz==0 → 0. Saturates at UINT32_MAX.
 *   uint32_t gj_adev_frames_from_us_5755(uint32_t latency_us,
 *                                        uint32_t rate_hz);
 *     - rate_hz * latency_us / 1000000 (integer, round half-up via
 *       +500000). rate_hz==0 → 0.
 *   uint32_t gj_batch_id_5755(void);
 *   __ aliases
 *   __libcgj_batch5755_marker = "libcgj-batch5755"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5755 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5755_marker[] = "libcgj-batch5755";

#define B5755_BATCH_ID  5755u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5755_sat_u32(uint64_t u64V)
{
	if (u64V > (uint64_t)0xffffffffu) {
		return 0xffffffffu;
	}
	return (uint32_t)u64V;
}

static uint32_t
b5755_from_ms(uint32_t u32Ms, uint32_t u32Rate)
{
	uint64_t u64Num;

	if (u32Rate == 0u) {
		return 0u;
	}
	/* half-up: (rate * ms + 500) / 1000 */
	u64Num = (uint64_t)u32Rate * (uint64_t)u32Ms + 500ull;
	return b5755_sat_u32(u64Num / 1000ull);
}

static uint32_t
b5755_from_us(uint32_t u32Us, uint32_t u32Rate)
{
	uint64_t u64Num;

	if (u32Rate == 0u) {
		return 0u;
	}
	/* half-up: (rate * us + 500000) / 1000000 */
	u64Num = (uint64_t)u32Rate * (uint64_t)u32Us + 500000ull;
	return b5755_sat_u32(u64Num / 1000000ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_frames_from_ms_5755 - latency budget (ms) → frame count.
 */
uint32_t
gj_adev_frames_from_ms_5755(uint32_t latency_ms, uint32_t rate_hz)
{
	(void)NULL;
	return b5755_from_ms(latency_ms, rate_hz);
}

/*
 * gj_adev_frames_from_us_5755 - latency budget (us) → frame count.
 */
uint32_t
gj_adev_frames_from_us_5755(uint32_t latency_us, uint32_t rate_hz)
{
	return b5755_from_us(latency_us, rate_hz);
}

uint32_t
gj_batch_id_5755(void)
{
	return B5755_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_frames_from_ms_5755(uint32_t latency_ms, uint32_t rate_hz)
    __attribute__((alias("gj_adev_frames_from_ms_5755")));

uint32_t __gj_adev_frames_from_us_5755(uint32_t latency_us, uint32_t rate_hz)
    __attribute__((alias("gj_adev_frames_from_us_5755")));

uint32_t __gj_batch_id_5755(void)
    __attribute__((alias("gj_batch_id_5755")));
