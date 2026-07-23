/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1857: image layer size from row pitch * height.
 *
 * Surface (unique symbols):
 *   uint64_t gj_image_layer_size(uint64_t row_pitch, uint32_t height);
 *     — Bytes for one 2D layer / slice: row_pitch * height.
 *       Returns 0 when either factor is 0 or the product overflows.
 *       Combine with arrayLayers or depth for full subresource size.
 *   uint64_t __gj_image_layer_size  (alias)
 *   __libcgj_batch1857_marker = "libcgj-batch1857"
 *
 * GPU/buffer exclusive integer-hint wave (1851–1859); milestone 1860.
 * Distinct from gj_row_pitch_align / gj_buffer_stride_bytes. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1857_marker[] = "libcgj-batch1857";

/* ---- freestanding helpers ---------------------------------------------- */

/* Checked row_pitch * height. */
static uint64_t
b1857_layer_size(uint64_t u64RowPitch, uint32_t u32Height)
{
	if (u64RowPitch == 0u || u32Height == 0u) {
		return 0u;
	}
	if (u64RowPitch > (UINT64_MAX / (uint64_t)u32Height)) {
		return 0u;
	}
	return u64RowPitch * (uint64_t)u32Height;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_image_layer_size — bytes for one image layer / depth slice.
 *
 * row_pitch: aligned bytes per row (from gj_row_pitch_align or similar)
 * height:    rows (texels or block rows)
 * Returns product, or 0 on zero inputs / overflow.
 */
uint64_t
gj_image_layer_size(uint64_t u64RowPitch, uint32_t u32Height)
{
	(void)sizeof(NULL);
	return b1857_layer_size(u64RowPitch, u32Height);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_image_layer_size(uint64_t u64RowPitch, uint32_t u32Height)
    __attribute__((alias("gj_image_layer_size")));
