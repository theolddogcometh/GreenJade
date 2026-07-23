/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6119: nested output subpixel order soft (_6119).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_subpixel_6119(uint32_t order);
 *     - Clamp wl_output-style subpixel order to [0, 5]:
 *         0 unknown, 1 none, 2 horizontal RGB, 3 horizontal BGR,
 *         4 vertical RGB, 5 vertical BGR. Above 5 → 0.
 *   uint32_t __gj_nested_out_subpixel_6119  (alias)
 *   __libcgj_batch6119_marker = "libcgj-batch6119"
 *
 * Exclusive continuum CREATE-ONLY (6111-6120: gamescope nested output —
 * nested_out_mode_pack_6111, nested_out_scale_6112, nested_out_xform_6113,
 * nested_out_refresh_hz_6114, nested_out_phys_mm_6115, nested_out_tag_6116,
 * nested_out_pos_pack_6117, nested_out_bpp_6118, nested_out_subpixel_6119,
 * batch_id_6120). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6119_marker[] = "libcgj-batch6119";

#define B6119_SUBPIX_MAX  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6119_clamp(uint32_t u32Order)
{
	if (u32Order > B6119_SUBPIX_MAX) {
		return 0u;
	}
	return u32Order;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_subpixel_6119 - clamp nested output subpixel order.
 *
 * order: soft subpixel enum (0..5 accepted)
 *
 * Returns order if in range, else 0 (unknown). Pure integer; no font
 * raster path. No parent wires.
 */
uint32_t
gj_nested_out_subpixel_6119(uint32_t u32Order)
{
	(void)NULL;
	return b6119_clamp(u32Order);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_subpixel_6119(uint32_t u32Order)
    __attribute__((alias("gj_nested_out_subpixel_6119")));
