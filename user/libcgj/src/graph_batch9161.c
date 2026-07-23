/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9161: soft Matroska/WebM EBML header id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkv_ebml_id_9161(void);
 *     - Returns 0x1A45DFA3 (EBML header element id soft catalog value).
 *       Pure-data container id stub; does not parse Matroska/WebM or
 *       open media files.
 *   uint32_t __gj_mkv_ebml_id_9161  (alias)
 *   __libcgj_batch9161_marker = "libcgj-batch9161"
 *
 * Exclusive continuum CREATE-ONLY (9161-9170: media container soft id
 * stubs — mkv_ebml_id_9161, mp4_ftyp_ok_u_9162, webm_ready_u_9163,
 * avi_riff_ok_u_9164, ogg_ready_u_9165, wav_riff_ok_u_9166,
 * flac_ready_u_9167, opus_ready_u_9168, media_ready_u_9169,
 * batch_id_9170).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No media demuxer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9161_marker[] = "libcgj-batch9161";

/* Soft EBML header element id (Matroska/WebM: 1A 45 DF A3). */
#define B9161_MKV_EBML_ID  0x1A45DFA3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9161_ebml_id(void)
{
	return B9161_MKV_EBML_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkv_ebml_id_9161 - soft Matroska/WebM EBML header catalog id.
 *
 * Always returns 0x1A45DFA3. Soft continuum constant; does not parse
 * EBML streams or open media files. No parent wires.
 */
uint32_t
gj_mkv_ebml_id_9161(void)
{
	(void)NULL;
	return b9161_ebml_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkv_ebml_id_9161(void)
    __attribute__((alias("gj_mkv_ebml_id_9161")));
