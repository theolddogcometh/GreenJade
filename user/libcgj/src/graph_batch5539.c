/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5539: 2D integer scale fit (min of axes) (_5539).
 *
 * Surface (unique symbols):
 *   uint32_t gj_integer_scale_fit_5539(uint32_t src_w, uint32_t src_h,
 *                                      uint32_t dst_w, uint32_t dst_h);
 *     - Largest integer scale s >= 1 that fits src into dst on both
 *       axes: min(floor(dst_w/src_w), floor(dst_h/src_h)). Any zero
 *       dimension → 0. If either axis cannot fit (src > dst) → 0.
 *   uint32_t __gj_integer_scale_fit_5539  (alias)
 *   __libcgj_batch5539_marker = "libcgj-batch5539"
 *
 * Exclusive continuum CREATE-ONLY (5531-5540: gamescope compositor —
 * gscope_comp_stub_5531, nested_refresh_hz_5532, fsr_sharpness_5533,
 * integer_scale_5534, deck_ui_chrome_5535, gscope_layer_z_5536,
 * nested_refresh_us_5537, fsr_sharp_q8_5538, integer_scale_fit_5539,
 * batch_id_5540). Unique surface only; no multi-def. Complements
 * gj_integer_scale_5534 (1D). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5539_marker[] = "libcgj-batch5539";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5539_axis(uint32_t u32Src, uint32_t u32Dst)
{
	if (u32Src == 0u || u32Dst == 0u) {
		return 0u;
	}
	if (u32Src > u32Dst) {
		return 0u;
	}
	return u32Dst / u32Src;
}

static uint32_t
b5539_fit(uint32_t u32SrcW, uint32_t u32SrcH,
    uint32_t u32DstW, uint32_t u32DstH)
{
	uint32_t u32Sx;
	uint32_t u32Sy;

	u32Sx = b5539_axis(u32SrcW, u32DstW);
	u32Sy = b5539_axis(u32SrcH, u32DstH);
	if (u32Sx == 0u || u32Sy == 0u) {
		return 0u;
	}
	if (u32Sx < u32Sy) {
		return u32Sx;
	}
	return u32Sy;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_integer_scale_fit_5539 - 2D integer scale that fits src in dst.
 *
 * src_w, src_h: source size in pixels
 * dst_w, dst_h: destination size in pixels
 *
 * Returns min of per-axis floor scales, or 0 if either axis fails.
 * Gamescope-style integer scaling helper. No parent wires.
 */
uint32_t
gj_integer_scale_fit_5539(uint32_t u32SrcW, uint32_t u32SrcH,
    uint32_t u32DstW, uint32_t u32DstH)
{
	(void)NULL;
	return b5539_fit(u32SrcW, u32SrcH, u32DstW, u32DstH);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_integer_scale_fit_5539(uint32_t u32SrcW, uint32_t u32SrcH,
    uint32_t u32DstW, uint32_t u32DstH)
    __attribute__((alias("gj_integer_scale_fit_5539")));
