/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1678: common PCM sample-rate acceptability hint.
 *
 * Surface (unique symbols):
 *   int gj_sample_rate_ok_hint(uint32_t rate);
 *     — Return 1 if rate is a common PCM rate used as a soft policy
 *       hint (8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000,
 *       88200, 96000, 176400, 192000). Else 0. Not a hard ABI gate —
 *       callers may still accept other rates.
 *   int __gj_sample_rate_ok_hint  (alias)
 *   __libcgj_batch1678_marker = "libcgj-batch1678"
 *
 * Audio/sample exclusive integer wave. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1678_marker[] = "libcgj-batch1678";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Linear scan of a small fixed table of widely supported PCM rates.
 * Static so the TU stays freestanding with no external tables.
 */
static int
b1678_rate_in_table(uint32_t u32Rate)
{
	static const uint32_t s_aRates[] = {
		8000u, 11025u, 16000u, 22050u, 24000u, 32000u,
		44100u, 48000u, 88200u, 96000u, 176400u, 192000u
	};
	size_t iRate;
	size_t cRates;

	cRates = sizeof(s_aRates) / sizeof(s_aRates[0]);
	for (iRate = 0u; iRate < cRates; iRate++) {
		if (s_aRates[iRate] == u32Rate) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sample_rate_ok_hint — 1 if rate is a common PCM sample rate.
 *
 * Soft hint for device/policy code; does not allocate or consult HW.
 */
int
gj_sample_rate_ok_hint(uint32_t u32Rate)
{
	return b1678_rate_in_table(u32Rate);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_sample_rate_ok_hint(uint32_t u32Rate)
    __attribute__((alias("gj_sample_rate_ok_hint")));
