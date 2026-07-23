/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9025: PipeWire sample-rate ok soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_rate_ok_u_9025(uint32_t rate);
 *     - Return 1 if rate is a common PCM sample rate soft-accepted by
 *       PipeWire/Pulse-shaped paths (8000, 11025, 16000, 22050, 24000,
 *       32000, 44100, 48000, 88200, 96000, 176400, 192000). Else 0.
 *   uint32_t __gj_pw_rate_ok_u_9025  (alias)
 *   __libcgj_batch9025_marker = "libcgj-batch9025"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_rate_ok_u_9025 surface only; no
 * multi-def. Distinct from gj_sample_rate_ok_hint (batch1678) and
 * gj_pw_rate_48k_5441. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9025_marker[] = "libcgj-batch9025";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Linear scan of a small fixed table of widely supported PCM rates.
 * Static so the TU stays freestanding with no external tables.
 */
static uint32_t
b9025_pw_rate_ok(uint32_t u32Rate)
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
			return 1u;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_rate_ok_u_9025 - 1 if rate is a common soft-accepted PCM rate.
 *
 * rate: sample rate in Hz.
 * Returns 1 for table hits, else 0. Soft policy hint only; does not
 * open a PipeWire context or call libc. No parent wires.
 */
uint32_t
gj_pw_rate_ok_u_9025(uint32_t u32Rate)
{
	(void)NULL;
	return b9025_pw_rate_ok(u32Rate);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_rate_ok_u_9025(uint32_t u32Rate)
    __attribute__((alias("gj_pw_rate_ok_u_9025")));
