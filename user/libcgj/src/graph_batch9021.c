/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9021: PipeWire playback stream soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_stream_play_id_9021(void);
 *     - Return soft PipeWire/Pulse playback stream catalog id (1).
 *   uint32_t __gj_pw_stream_play_id_9021  (alias)
 *   __libcgj_batch9021_marker = "libcgj-batch9021"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_stream_play_id_9021 surface only; no
 * multi-def. Distinct from gj_pw_media_class_tag_6134 sink/source tags
 * and gj_pw_port_dir_mask_6138. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9021_marker[] = "libcgj-batch9021";

/* Soft playback stream catalog id (output / play path). */
#define B9021_PW_STREAM_PLAY_ID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9021_pw_stream_play_id(void)
{
	return B9021_PW_STREAM_PLAY_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_stream_play_id_9021 - soft PipeWire playback stream catalog id.
 *
 * Always returns 1 (playback / output stream kind). Soft pure-data
 * catalog id only; does not open a PipeWire stream or call libc.
 * No parent wires.
 */
uint32_t
gj_pw_stream_play_id_9021(void)
{
	(void)NULL;
	return b9021_pw_stream_play_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_stream_play_id_9021(void)
    __attribute__((alias("gj_pw_stream_play_id_9021")));
