/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9152: soft JPEG SOI catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_jpeg_soi_id_9152(void);
 *     - Returns 0xFFD8 (JPEG SOI soft catalog value). Pure-data marker
 *       id stub; does not parse JPEG or call libjpeg.
 *   uint32_t __gj_jpeg_soi_id_9152  (alias)
 *   __libcgj_batch9152_marker = "libcgj-batch9152"
 *
 * Exclusive continuum CREATE-ONLY (9151-9160: image codec soft id stubs —
 * png_sig_ok_u_9151, jpeg_soi_id_9152, gif_sig_ok_u_9153,
 * webp_ready_u_9154, bmp_header_ok_u_9155, tga_ready_u_9156,
 * ico_ready_u_9157, image_wh_ok_u_9158, image_ready_u_9159,
 * batch_id_9160).
 * Unique surface only; no multi-def. Distinct from
 * gj_png_sig_ok_u_9151 (batch9151). No parent wires. No __int128.
 * No image codec implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9152_marker[] = "libcgj-batch9152";

/* Soft JPEG Start-Of-Image marker catalog id (SOI = 0xFFD8). */
#define B9152_JPEG_SOI_ID  0xFFD8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9152_jpeg_soi_id(void)
{
	return B9152_JPEG_SOI_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jpeg_soi_id_9152 - soft JPEG SOI marker catalog id.
 *
 * Always returns 0xFFD8. Soft continuum constant; does not parse JPEG
 * or call libjpeg. No parent wires.
 */
uint32_t
gj_jpeg_soi_id_9152(void)
{
	(void)NULL;
	return b9152_jpeg_soi_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_jpeg_soi_id_9152(void)
    __attribute__((alias("gj_jpeg_soi_id_9152")));
