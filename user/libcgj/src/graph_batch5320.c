/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5320: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5320(void);
 *     - Returns the compile-time graph batch number for this TU (5320).
 *   uint32_t __gj_batch_id_5320  (alias)
 *   __libcgj_batch5320_marker = "libcgj-batch5320"
 *
 * Exclusive continuum CREATE-ONLY (5311-5320: audio rate unique —
 * audio_rate_44k_u, audio_rate_48k_u, audio_rate_96k_u, audio_ch_mono_u,
 * audio_ch_stereo_u, audio_bps_16_u, audio_bps_24_u, audio_bps_32_u,
 * audio_frame_bytes_u, batch_id_5320). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5320_marker[] = "libcgj-batch5320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5320_id(void)
{
	return 5320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5320 - report this TU's graph batch number.
 *
 * Always returns 5320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5320(void)
{
	(void)NULL;
	return b5320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5320(void)
    __attribute__((alias("gj_batch_id_5320")));
