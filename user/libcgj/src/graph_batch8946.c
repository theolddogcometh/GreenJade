/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8946: soft ALSA PCM rate-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcm_rate_ok_u_8946(void);
 *     - Returns 1 (PCM sample-rate soft-id continuum ok). Pure-data
 *       product tag; does not set rates or call libasound.
 *   uint32_t __gj_pcm_rate_ok_u_8946  (alias)
 *   __libcgj_batch8946_marker = "libcgj-batch8946"
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

const char __libcgj_batch8946_marker[] = "libcgj-batch8946";

#define B8946_PCM_RATE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8946_rate_ok(void)
{
	return B8946_PCM_RATE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcm_rate_ok_u_8946 - PCM sample-rate soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not configure
 * sample rates or call libasound. No parent wires.
 */
uint32_t
gj_pcm_rate_ok_u_8946(void)
{
	(void)NULL;
	return b8946_rate_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcm_rate_ok_u_8946(void)
    __attribute__((alias("gj_pcm_rate_ok_u_8946")));
