/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4870: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4870(void);
 *     - Returns the compile-time graph batch number for this TU (4870).
 *   uint32_t __gj_batch_id_4870  (alias)
 *   __libcgj_batch4870_marker = "libcgj-batch4870"
 *
 * Exclusive continuum CREATE-ONLY (4861-4870: pcm_s16_to_u16_u,
 * pcm_u16_to_s16_u, pcm_clamp_s16_u, pcm_mix_s16_u,
 * pcm_mono_from_stereo_u, pcm_frame_bytes_u, pcm_byte_rate_u,
 * pcm_frames_u, pcm_duration_ms_u, batch_id_4870). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4870_marker[] = "libcgj-batch4870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4870_id(void)
{
	return 4870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4870 - report this TU's graph batch number.
 *
 * Always returns 4870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4870(void)
{
	(void)NULL;
	return b4870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4870(void)
    __attribute__((alias("gj_batch_id_4870")));
