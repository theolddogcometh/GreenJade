/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9159: soft image codec id stub continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_image_ready_u_9159(void);
 *     - Returns 1 (image codec soft id stub continuum ready). Pure-data
 *       product tag that exclusive wave 9151-9160 surfaces are present.
 *   uint32_t __gj_image_ready_u_9159  (alias)
 *   __libcgj_batch9159_marker = "libcgj-batch9159"
 *
 * Exclusive continuum CREATE-ONLY (9151-9160: image codec soft id stubs —
 * png_sig_ok_u_9151, jpeg_soi_id_9152, gif_sig_ok_u_9153,
 * webp_ready_u_9154, bmp_header_ok_u_9155, tga_ready_u_9156,
 * ico_ready_u_9157, image_wh_ok_u_9158, image_ready_u_9159,
 * batch_id_9160).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No image codec implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9159_marker[] = "libcgj-batch9159";

#define B9159_IMAGE_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9159_image_ready(void)
{
	return B9159_IMAGE_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_image_ready_u_9159 - image codec soft id stub continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive wave
 * surfaces are present. Does not decode images or call codec libs.
 * No parent wires.
 */
uint32_t
gj_image_ready_u_9159(void)
{
	(void)NULL;
	return b9159_image_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_image_ready_u_9159(void)
    __attribute__((alias("gj_image_ready_u_9159")));
