/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5534: integer scale factor (gamescope) (_5534).
 *
 * Surface (unique symbols):
 *   uint32_t gj_integer_scale_5534(uint32_t src, uint32_t dst);
 *     - Largest integer scale factor s >= 1 such that src * s <= dst.
 *       src == 0 or dst == 0 → 0. If src > dst → 0 (does not fit).
 *       Saturates s at UINT32_MAX when src == 1 and dst is max.
 *   uint32_t __gj_integer_scale_5534  (alias)
 *   __libcgj_batch5534_marker = "libcgj-batch5534"
 *
 * Exclusive continuum CREATE-ONLY (5531-5540: gamescope compositor —
 * gscope_comp_stub_5531, nested_refresh_hz_5532, fsr_sharpness_5533,
 * integer_scale_5534, deck_ui_chrome_5535, gscope_layer_z_5536,
 * nested_refresh_us_5537, fsr_sharp_q8_5538, integer_scale_fit_5539,
 * batch_id_5540). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5534_marker[] = "libcgj-batch5534";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5534_iscale(uint32_t u32Src, uint32_t u32Dst)
{
	if (u32Src == 0u || u32Dst == 0u) {
		return 0u;
	}
	if (u32Src > u32Dst) {
		return 0u;
	}
	return u32Dst / u32Src;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_integer_scale_5534 - largest integer scale of src that fits in dst.
 *
 * src: source axis length in pixels (0 → 0)
 * dst: destination axis length in pixels (0 → 0)
 *
 * Returns floor(dst / src) when src fits in dst, else 0. 1D only; call
 * per axis and take min for 2D fit (see batch5539). No parent wires.
 */
uint32_t
gj_integer_scale_5534(uint32_t u32Src, uint32_t u32Dst)
{
	(void)NULL;
	return b5534_iscale(u32Src, u32Dst);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_integer_scale_5534(uint32_t u32Src, uint32_t u32Dst)
    __attribute__((alias("gj_integer_scale_5534")));
