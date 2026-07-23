/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14134: PipeWire buffer-frames clamp soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_buf_frames_clamp_14134(uint32_t frames);
 *     - Clamp soft period/buffer size in frames into the conventional
 *       stub range [32, 16384]. Values below 32 become 32; values
 *       above 16384 become 16384.
 *   uint32_t __gj_pw_buf_frames_clamp_14134  (alias)
 *   __libcgj_batch14134_marker = "libcgj-batch14134"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_buf_frames_clamp_14134 surface only;
 * no multi-def. Distinct from gj_pw_quantum_clamp_6136 ([16, 8192]).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14134_marker[] = "libcgj-batch14134";

#define B14134_FRAMES_MIN  32u
#define B14134_FRAMES_MAX  16384u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14134_buf_frames_clamp(uint32_t uFrames)
{
	if (uFrames < B14134_FRAMES_MIN) {
		return B14134_FRAMES_MIN;
	}
	if (uFrames > B14134_FRAMES_MAX) {
		return B14134_FRAMES_MAX;
	}
	return uFrames;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_buf_frames_clamp_14134 - clamp soft PipeWire buffer frames.
 *
 * frames: soft period/buffer size in PCM frames.
 * Returns frames clamped to [32, 16384]. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_buf_frames_clamp_14134(uint32_t uFrames)
{
	(void)NULL;
	return b14134_buf_frames_clamp(uFrames);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_buf_frames_clamp_14134(uint32_t uFrames)
    __attribute__((alias("gj_pw_buf_frames_clamp_14134")));
