/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8944: soft ALSA PCM playback stream id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_stream_play_id_8944(void);
 *     - Returns 0 (SND_PCM_STREAM_PLAYBACK soft catalog value).
 *       Pure-data stream id stub; does not open devices or play audio.
 *   uint32_t __gj_pcm_stream_play_id_8944  (alias)
 *   __libcgj_batch8944_marker = "libcgj-batch8944"
 *
 * Exclusive continuum CREATE-ONLY (8941-8950: alsa/pcm id stubs —
 * pcm_format_s16_id_8941, pcm_format_s32_id_8942,
 * pcm_access_rw_id_8943, pcm_stream_play_id_8944,
 * pcm_stream_cap_id_8945, pcm_rate_ok_u_8946, pcm_channels_ok_u_8947,
 * pcm_period_ok_u_8948, alsa_ready_u_8949, batch_id_8950).
 * Unique surface only; no multi-def. Distinct from
 * gj_pcm_stream_cap_id_8945 (batch8945). No parent wires. No __int128.
 * No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8944_marker[] = "libcgj-batch8944";

/* Soft ALSA PCM PLAYBACK stream catalog id (public enum value 0). */
#define B8944_PCM_STREAM_PLAY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8944_stream_play(void)
{
	return B8944_PCM_STREAM_PLAY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_stream_play_id_8944 - soft ALSA PCM PLAYBACK stream catalog id.
 *
 * Always returns 0. Soft continuum constant; does not open playback
 * streams or call libasound. No parent wires.
 */
uint32_t
gj_pcm_stream_play_id_8944(void)
{
	(void)NULL;
	return b8944_stream_play();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_stream_play_id_8944(void)
    __attribute__((alias("gj_pcm_stream_play_id_8944")));
