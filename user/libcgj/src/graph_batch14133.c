/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14133: PipeWire channel-count ok soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_channels_ok_u_14133(uint32_t channels);
 *     - Return 1 if channels is in the soft-accepted PipeWire/SPA
 *       range [1, 32], else 0.
 *   uint32_t __gj_pw_channels_ok_u_14133  (alias)
 *   __libcgj_batch14133_marker = "libcgj-batch14133"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_channels_ok_u_14133 surface only; no
 * multi-def. Distinct from gj_pw_rate_ok_u_9025 (sample rate). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14133_marker[] = "libcgj-batch14133";

#define B14133_CH_MIN  1u
#define B14133_CH_MAX  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14133_channels_ok(uint32_t uCh)
{
	if (uCh < B14133_CH_MIN) {
		return 0u;
	}
	if (uCh > B14133_CH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_channels_ok_u_14133 - 1 if channel count is soft-accepted.
 *
 * channels: PCM channel count.
 * Returns 1 for [1, 32], else 0. Soft policy hint only; does not open
 * a PipeWire context or call libc. No parent wires.
 */
uint32_t
gj_pw_channels_ok_u_14133(uint32_t uChannels)
{
	(void)NULL;
	return b14133_channels_ok(uChannels);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_channels_ok_u_14133(uint32_t uChannels)
    __attribute__((alias("gj_pw_channels_ok_u_14133")));
