/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8941: soft ALSA PCM S16 format id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_format_s16_id_8941(void);
 *     - Returns 2 (SND_PCM_FORMAT_S16_LE soft catalog value). Pure-data
 *       format id stub; does not open devices or call libasound.
 *   uint32_t __gj_pcm_format_s16_id_8941  (alias)
 *   __libcgj_batch8941_marker = "libcgj-batch8941"
 *
 * Exclusive continuum CREATE-ONLY (8941-8950: alsa/pcm id stubs —
 * pcm_format_s16_id_8941, pcm_format_s32_id_8942,
 * pcm_access_rw_id_8943, pcm_stream_play_id_8944,
 * pcm_stream_cap_id_8945, pcm_rate_ok_u_8946, pcm_channels_ok_u_8947,
 * pcm_period_ok_u_8948, alsa_ready_u_8949, batch_id_8950).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8941_marker[] = "libcgj-batch8941";

/* Soft ALSA PCM S16_LE format catalog id (public enum value 2). */
#define B8941_PCM_S16_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8941_s16_id(void)
{
	return B8941_PCM_S16_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_format_s16_id_8941 - soft ALSA PCM S16_LE format catalog id.
 *
 * Always returns 2. Soft continuum constant; does not open PCM
 * devices or call libasound. No parent wires.
 */
uint32_t
gj_pcm_format_s16_id_8941(void)
{
	(void)NULL;
	return b8941_s16_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_format_s16_id_8941(void)
    __attribute__((alias("gj_pcm_format_s16_id_8941")));
