/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5752: audio sink sample-rate clamp / prefer.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adev_sink_rate_clamp_5752(uint32_t rate_hz);
 *     - Snap rate_hz into the standard sink table
 *       {8000,11025,16000,22050,32000,44100,48000,88200,96000,192000}.
 *       0 or below-table → 8000; above max → 192000; else nearest entry.
 *   uint32_t gj_adev_sink_rate_prefer_5752(uint32_t want_hz, uint32_t cap_hz);
 *     - Prefer want_hz if it clamps to a rate <= cap_hz (cap 0 → no cap);
 *       else return the largest table rate <= cap_hz (or clamp of want).
 *   uint32_t gj_batch_id_5752(void);
 *   __ aliases
 *   __libcgj_batch5752_marker = "libcgj-batch5752"
 *
 * Exclusive continuum CREATE-ONLY (5751-5760: audio device graph,
 * sink/source rates, latency ms stubs). Unique *_5752 surfaces only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5752_marker[] = "libcgj-batch5752";

#define B5752_BATCH_ID  5752u
#define B5752_N_RATES   10u

static const uint32_t s_b5752_rates[B5752_N_RATES] = {
	8000u, 11025u, 16000u, 22050u, 32000u,
	44100u, 48000u, 88200u, 96000u, 192000u
};

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5752_abs_diff(uint32_t u32A, uint32_t u32B)
{
	return (u32A >= u32B) ? (u32A - u32B) : (u32B - u32A);
}

static uint32_t
b5752_clamp(uint32_t u32Rate)
{
	uint32_t u32I;
	uint32_t u32Best;
	uint32_t u32BestD;
	uint32_t u32D;

	if (u32Rate == 0u) {
		return s_b5752_rates[0];
	}
	if (u32Rate <= s_b5752_rates[0]) {
		return s_b5752_rates[0];
	}
	if (u32Rate >= s_b5752_rates[B5752_N_RATES - 1u]) {
		return s_b5752_rates[B5752_N_RATES - 1u];
	}

	u32Best = s_b5752_rates[0];
	u32BestD = b5752_abs_diff(u32Rate, u32Best);
	for (u32I = 1u; u32I < B5752_N_RATES; u32I++) {
		u32D = b5752_abs_diff(u32Rate, s_b5752_rates[u32I]);
		if (u32D < u32BestD) {
			u32BestD = u32D;
			u32Best = s_b5752_rates[u32I];
		}
	}
	return u32Best;
}

static uint32_t
b5752_prefer(uint32_t u32Want, uint32_t u32Cap)
{
	uint32_t u32Clamped;
	uint32_t u32I;
	uint32_t u32Best;

	u32Clamped = b5752_clamp(u32Want);
	if (u32Cap == 0u) {
		return u32Clamped;
	}
	if (u32Clamped <= u32Cap) {
		return u32Clamped;
	}

	/* Largest table rate <= cap. */
	u32Best = s_b5752_rates[0];
	for (u32I = 0u; u32I < B5752_N_RATES; u32I++) {
		if (s_b5752_rates[u32I] <= u32Cap) {
			u32Best = s_b5752_rates[u32I];
		}
	}
	return u32Best;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adev_sink_rate_clamp_5752 - snap a requested sink rate to the
 * standard playback table.
 */
uint32_t
gj_adev_sink_rate_clamp_5752(uint32_t rate_hz)
{
	(void)NULL;
	return b5752_clamp(rate_hz);
}

/*
 * gj_adev_sink_rate_prefer_5752 - negotiate want_hz under optional cap.
 */
uint32_t
gj_adev_sink_rate_prefer_5752(uint32_t want_hz, uint32_t cap_hz)
{
	return b5752_prefer(want_hz, cap_hz);
}

uint32_t
gj_batch_id_5752(void)
{
	return B5752_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_adev_sink_rate_clamp_5752(uint32_t rate_hz)
    __attribute__((alias("gj_adev_sink_rate_clamp_5752")));

uint32_t __gj_adev_sink_rate_prefer_5752(uint32_t want_hz, uint32_t cap_hz)
    __attribute__((alias("gj_adev_sink_rate_prefer_5752")));

uint32_t __gj_batch_id_5752(void)
    __attribute__((alias("gj_batch_id_5752")));
