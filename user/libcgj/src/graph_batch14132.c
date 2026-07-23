/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14132: PipeWire SPA F32LE format soft id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_spa_fmt_f32le_id_14132(void);
 *     - Return soft SPA audio raw F32LE format catalog id (2).
 *   uint32_t __gj_pw_spa_fmt_f32le_id_14132  (alias)
 *   __libcgj_batch14132_marker = "libcgj-batch14132"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_spa_fmt_f32le_id_14132 surface only;
 * no multi-def. Distinct from gj_pw_format_f32_id_9024 (batch9024) and
 * gj_pw_spa_fmt_s16le_id_14131. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14132_marker[] = "libcgj-batch14132";

/* Soft SPA audio raw F32LE format catalog id. */
#define B14132_SPA_FMT_F32LE_ID  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14132_spa_fmt_f32le_id(void)
{
	return B14132_SPA_FMT_F32LE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_spa_fmt_f32le_id_14132 - soft SPA F32LE format catalog id.
 *
 * Always returns 2. Soft pure-data format tag only; does not open a
 * stream or negotiate SPA params. No float arithmetic. No parent wires.
 */
uint32_t
gj_pw_spa_fmt_f32le_id_14132(void)
{
	(void)NULL;
	return b14132_spa_fmt_f32le_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_spa_fmt_f32le_id_14132(void)
    __attribute__((alias("gj_pw_spa_fmt_f32le_id_14132")));
