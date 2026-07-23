/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14131: PipeWire SPA S16LE format soft id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_spa_fmt_s16le_id_14131(void);
 *     - Return soft SPA audio raw S16LE format catalog id (1).
 *   uint32_t __gj_pw_spa_fmt_s16le_id_14131  (alias)
 *   __libcgj_batch14131_marker = "libcgj-batch14131"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_spa_fmt_s16le_id_14131 surface only;
 * no multi-def. Distinct from gj_pw_format_s16_id_9023 (batch9023).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14131_marker[] = "libcgj-batch14131";

/* Soft SPA audio raw S16LE format catalog id. */
#define B14131_SPA_FMT_S16LE_ID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14131_spa_fmt_s16le_id(void)
{
	return B14131_SPA_FMT_S16LE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_spa_fmt_s16le_id_14131 - soft SPA S16LE format catalog id.
 *
 * Always returns 1. Soft pure-data format tag only; does not open a
 * PipeWire stream or negotiate SPA params. No parent wires.
 */
uint32_t
gj_pw_spa_fmt_s16le_id_14131(void)
{
	(void)NULL;
	return b14131_spa_fmt_s16le_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_spa_fmt_s16le_id_14131(void)
    __attribute__((alias("gj_pw_spa_fmt_s16le_id_14131")));
