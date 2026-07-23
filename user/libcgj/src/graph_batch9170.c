/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9170: media container soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9170(void);
 *     - Returns the compile-time graph batch number for this TU (9170).
 *   uint32_t __gj_batch_id_9170  (alias)
 *   __libcgj_batch9170_marker = "libcgj-batch9170"
 *
 * Exclusive continuum CREATE-ONLY (9161-9170: media container soft id
 * stubs — mkv_ebml_id_9161, mp4_ftyp_ok_u_9162, webm_ready_u_9163,
 * avi_riff_ok_u_9164, ogg_ready_u_9165, wav_riff_ok_u_9166,
 * flac_ready_u_9167, opus_ready_u_9168, media_ready_u_9169,
 * batch_id_9170).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No media demuxer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9170_marker[] = "libcgj-batch9170";

#define B9170_BATCH_ID  9170u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9170_id(void)
{
	return B9170_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9170 - report this TU's graph batch number.
 *
 * Always returns 9170. Link-time presence tags the exclusive media
 * container soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9170(void)
{
	(void)NULL;
	return b9170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9170(void)
    __attribute__((alias("gj_batch_id_9170")));
