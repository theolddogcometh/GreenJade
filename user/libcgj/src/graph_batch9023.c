/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9023: PipeWire S16 PCM format soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_format_s16_id_9023(void);
 *     - Return soft S16 PCM format catalog id (16).
 *   uint32_t __gj_pw_format_s16_id_9023  (alias)
 *   __libcgj_batch9023_marker = "libcgj-batch9023"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_format_s16_id_9023 surface only; no
 * multi-def. Distinct from gj_alsa_fmt_s16_bps_5444 (bits-per-sample)
 * and gj_audio_bps_16_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9023_marker[] = "libcgj-batch9023";

/* Soft signed 16-bit PCM format catalog id (bit-width mnemonic). */
#define B9023_PW_FMT_S16_ID  ((uint32_t)16u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9023_pw_format_s16_id(void)
{
	return B9023_PW_FMT_S16_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_format_s16_id_9023 - soft PipeWire S16 PCM format catalog id.
 *
 * Always returns 16. Soft pure-data format tag only; does not open a
 * stream or negotiate SPA params. No parent wires.
 */
uint32_t
gj_pw_format_s16_id_9023(void)
{
	(void)NULL;
	return b9023_pw_format_s16_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_format_s16_id_9023(void)
    __attribute__((alias("gj_pw_format_s16_id_9023")));
