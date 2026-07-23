/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6111: nested output mode pack (WxH) (_6111).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_mode_pack_6111(uint32_t width, uint32_t height);
 *     - Pack nested (inner) wl_output-style mode: (w & 0xFFFF) << 16 |
 *       (h & 0xFFFF). Soft 16-bit clamp per axis; no Wayland I/O.
 *   uint32_t __gj_nested_out_mode_pack_6111  (alias)
 *   __libcgj_batch6111_marker = "libcgj-batch6111"
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

const char __libcgj_batch6111_marker[] = "libcgj-batch6111";

#define B6111_AXIS_MASK  0x0000FFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6111_pack(uint32_t u32W, uint32_t u32H)
{
	return ((u32W & B6111_AXIS_MASK) << 16) | (u32H & B6111_AXIS_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_mode_pack_6111 - pack nested output mode WxH.
 *
 * width, height: soft pixel extents (low 16 bits retained each)
 *
 * Returns (w16 << 16) | h16. Gamescope nested output mode stub; no
 * connector enumeration. No parent wires.
 */
uint32_t
gj_nested_out_mode_pack_6111(uint32_t u32Width, uint32_t u32Height)
{
	(void)NULL;
	return b6111_pack(u32Width, u32Height);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_mode_pack_6111(uint32_t u32Width, uint32_t u32Height)
    __attribute__((alias("gj_nested_out_mode_pack_6111")));
