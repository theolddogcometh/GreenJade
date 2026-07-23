/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9162: soft MP4/ISO-BMFF ftyp-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mp4_ftyp_ok_u_9162(void);
 *     - Returns 1 (mp4 ftyp soft-id continuum ok). Pure-data product
 *       tag; does not inspect ftyp boxes or open media files.
 *   uint32_t __gj_mp4_ftyp_ok_u_9162  (alias)
 *   __libcgj_batch9162_marker = "libcgj-batch9162"
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

const char __libcgj_batch9162_marker[] = "libcgj-batch9162";

#define B9162_MP4_FTYP_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9162_ftyp_ok(void)
{
	return B9162_MP4_FTYP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mp4_ftyp_ok_u_9162 - MP4 ftyp soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not inspect ISO
 * BMFF boxes or open media files. No parent wires.
 */
uint32_t
gj_mp4_ftyp_ok_u_9162(void)
{
	(void)NULL;
	return b9162_ftyp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mp4_ftyp_ok_u_9162(void)
    __attribute__((alias("gj_mp4_ftyp_ok_u_9162")));
