/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9151: soft PNG signature-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_png_sig_ok_u_9151(void);
 *     - Returns 1 (PNG signature soft-id continuum ok). Pure-data product
 *       tag; does not parse PNG or call libpng.
 *   uint32_t __gj_png_sig_ok_u_9151  (alias)
 *   __libcgj_batch9151_marker = "libcgj-batch9151"
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

const char __libcgj_batch9151_marker[] = "libcgj-batch9151";

#define B9151_PNG_SIG_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9151_png_sig_ok(void)
{
	return B9151_PNG_SIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_png_sig_ok_u_9151 - PNG signature soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse PNG
 * magic bytes or call libpng. No parent wires.
 */
uint32_t
gj_png_sig_ok_u_9151(void)
{
	(void)NULL;
	return b9151_png_sig_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_png_sig_ok_u_9151(void)
    __attribute__((alias("gj_png_sig_ok_u_9151")));
