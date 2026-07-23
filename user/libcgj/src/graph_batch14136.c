/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14136: PipeWire stream-state normalize stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_stream_state_norm_14136(uint32_t state);
 *     - Normalize a soft stream-state code into a closed set:
 *         0 = error, 1 = unconnected, 2 = connecting, 3 = paused,
 *         4 = streaming
 *       Values above 4 clamp to 0 (error). Soft pure-data only.
 *   uint32_t __gj_pw_stream_state_norm_14136  (alias)
 *   __libcgj_batch14136_marker = "libcgj-batch14136"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_stream_state_norm_14136 surface only;
 * no multi-def. Distinct from gj_pw_node_state_norm_6135 (node states).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14136_marker[] = "libcgj-batch14136";

#define B14136_STATE_MAX  4u /* streaming */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14136_stream_state_norm(uint32_t uState)
{
	if (uState > B14136_STATE_MAX) {
		return 0u;
	}
	return uState;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_stream_state_norm_14136 - normalize PipeWire-shaped stream state.
 *
 * state: soft stream state code (0..4 valid; others → 0 error).
 * Returns normalized state in [0, 4]. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_stream_state_norm_14136(uint32_t uState)
{
	(void)NULL;
	return b14136_stream_state_norm(uState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_stream_state_norm_14136(uint32_t uState)
    __attribute__((alias("gj_pw_stream_state_norm_14136")));
