/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14135: PipeWire latency soft pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_latency_nsec_pack_14135(uint32_t rate,
 *                                          uint32_t frames);
 *     - Soft latency key from sample rate and buffer frames:
 *         [31:16] rate lo16 | [15:0] frames lo16
 *       Each field is masked to 16 bits (no nsec math; pure pack).
 *   uint32_t __gj_pw_latency_nsec_pack_14135  (alias)
 *   __libcgj_batch14135_marker = "libcgj-batch14135"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_latency_nsec_pack_14135 surface only;
 * no multi-def. Distinct from gj_pw_graph_counts_pack_6137. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14135_marker[] = "libcgj-batch14135";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14135_latency_pack(uint32_t uRate, uint32_t uFrames)
{
	return ((uRate & 0xFFFFu) << 16) | (uFrames & 0xFFFFu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_latency_nsec_pack_14135 - pack soft rate/frames latency key.
 *
 * rate: sample rate in Hz (low 16 bits used).
 * frames: buffer/period size in frames (low 16 bits used).
 * Returns packed 32-bit stub key. Soft pure-data only; no nsec
 * arithmetic and no PipeWire connection. No parent wires.
 */
uint32_t
gj_pw_latency_nsec_pack_14135(uint32_t uRate, uint32_t uFrames)
{
	(void)NULL;
	return b14135_latency_pack(uRate, uFrames);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_latency_nsec_pack_14135(uint32_t uRate, uint32_t uFrames)
    __attribute__((alias("gj_pw_latency_nsec_pack_14135")));
