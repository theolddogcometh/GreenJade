/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14140(void);
 *     - Returns the compile-time graph batch number for this TU (14140).
 *   uint32_t __gj_batch_id_14140  (alias)
 *   __libcgj_batch14140_marker = "libcgj-batch14140"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14140_marker[] = "libcgj-batch14140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14140_id(void)
{
	return 14140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14140 - report this TU's graph batch number.
 *
 * Always returns 14140. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_14140(void)
{
	(void)NULL;
	return b14140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14140(void)
    __attribute__((alias("gj_batch_id_14140")));
