/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9155: soft BMP header-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bmp_header_ok_u_9155(void);
 *     - Returns 1 (BMP header soft-id continuum ok). Pure-data product
 *       tag; does not parse BMP or call image libraries.
 *   uint32_t __gj_bmp_header_ok_u_9155  (alias)
 *   __libcgj_batch9155_marker = "libcgj-batch9155"
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

const char __libcgj_batch9155_marker[] = "libcgj-batch9155";

#define B9155_BMP_HEADER_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9155_bmp_header_ok(void)
{
	return B9155_BMP_HEADER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bmp_header_ok_u_9155 - BMP header soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse BITMAP
 * headers or call image libraries. No parent wires.
 */
uint32_t
gj_bmp_header_ok_u_9155(void)
{
	(void)NULL;
	return b9155_bmp_header_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bmp_header_ok_u_9155(void)
    __attribute__((alias("gj_bmp_header_ok_u_9155")));
