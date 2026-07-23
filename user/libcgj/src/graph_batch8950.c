/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8950: alsa/pcm soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8950(void);
 *     - Returns the compile-time graph batch number for this TU (8950).
 *   uint32_t __gj_batch_id_8950  (alias)
 *   __libcgj_batch8950_marker = "libcgj-batch8950"
 *
 * Exclusive continuum CREATE-ONLY (8941-8950: alsa/pcm id stubs —
 * pcm_format_s16_id_8941, pcm_format_s32_id_8942,
 * pcm_access_rw_id_8943, pcm_stream_play_id_8944,
 * pcm_stream_cap_id_8945, pcm_rate_ok_u_8946, pcm_channels_ok_u_8947,
 * pcm_period_ok_u_8948, alsa_ready_u_8949, batch_id_8950).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No ALSA implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8950_marker[] = "libcgj-batch8950";

#define B8950_BATCH_ID  8950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8950_id(void)
{
	return B8950_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8950 - report this TU's graph batch number.
 *
 * Always returns 8950. Link-time presence tags the exclusive
 * alsa/pcm soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_8950(void)
{
	(void)NULL;
	return b8950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8950(void)
    __attribute__((alias("gj_batch_id_8950")));
