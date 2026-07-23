/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6117: nested output position pack (x,y) (_6117).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_pos_pack_6117(uint32_t x, uint32_t y);
 *     - Pack nested-output origin: (x & 0xFFFF) << 16 | (y & 0xFFFF).
 *       Soft layout position for multi-output nested stacks; no DRM.
 *   uint32_t __gj_nested_out_pos_pack_6117  (alias)
 *   __libcgj_batch6117_marker = "libcgj-batch6117"
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

const char __libcgj_batch6117_marker[] = "libcgj-batch6117";

#define B6117_COORD_MASK  0x0000FFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6117_pack(uint32_t u32X, uint32_t u32Y)
{
	return ((u32X & B6117_COORD_MASK) << 16) | (u32Y & B6117_COORD_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_pos_pack_6117 - pack nested output layout origin (x,y).
 *
 * x, y: soft pixel origins (low 16 bits each)
 *
 * Returns (x16 << 16) | y16. Pure integer layout key; no seat focus.
 * No parent wires.
 */
uint32_t
gj_nested_out_pos_pack_6117(uint32_t u32X, uint32_t u32Y)
{
	(void)NULL;
	return b6117_pack(u32X, u32Y);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_pos_pack_6117(uint32_t u32X, uint32_t u32Y)
    __attribute__((alias("gj_nested_out_pos_pack_6117")));
