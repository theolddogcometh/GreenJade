/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8943: soft ALSA PCM RW access id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_access_rw_id_8943(void);
 *     - Returns 3 (SND_PCM_ACCESS_RW_INTERLEAVED soft catalog value).
 *       Pure-data access id stub; does not configure PCM or call
 *       libasound.
 *   uint32_t __gj_pcm_access_rw_id_8943  (alias)
 *   __libcgj_batch8943_marker = "libcgj-batch8943"
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

const char __libcgj_batch8943_marker[] = "libcgj-batch8943";

/* Soft ALSA PCM RW_INTERLEAVED access catalog id (public enum value 3). */
#define B8943_PCM_ACCESS_RW  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8943_access_rw(void)
{
	return B8943_PCM_ACCESS_RW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_access_rw_id_8943 - soft ALSA PCM RW_INTERLEAVED access id.
 *
 * Always returns 3. Soft continuum constant; does not configure PCM
 * access modes or call libasound. No parent wires.
 */
uint32_t
gj_pcm_access_rw_id_8943(void)
{
	(void)NULL;
	return b8943_access_rw();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_access_rw_id_8943(void)
    __attribute__((alias("gj_pcm_access_rw_id_8943")));
