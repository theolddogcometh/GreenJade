/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6115: nested output physical size mm pack (_6115).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_phys_mm_6115(uint32_t width_mm, uint32_t height_mm);
 *     - Pack soft physical size in millimetres: (w & 0xFFFF) << 16 |
 *       (h & 0xFFFF). Mirrors wl_output geometry physical axes; no EDID.
 *   uint32_t __gj_nested_out_phys_mm_6115  (alias)
 *   __libcgj_batch6115_marker = "libcgj-batch6115"
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

const char __libcgj_batch6115_marker[] = "libcgj-batch6115";

#define B6115_MM_MASK  0x0000FFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6115_pack(uint32_t u32Wmm, uint32_t u32Hmm)
{
	return ((u32Wmm & B6115_MM_MASK) << 16) | (u32Hmm & B6115_MM_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_phys_mm_6115 - pack nested output physical size (mm).
 *
 * width_mm, height_mm: soft physical axes (low 16 bits each)
 *
 * Returns (w16 << 16) | h16. Pure integer geometry stub; no panel
 * probe. No parent wires.
 */
uint32_t
gj_nested_out_phys_mm_6115(uint32_t u32WidthMm, uint32_t u32HeightMm)
{
	(void)NULL;
	return b6115_pack(u32WidthMm, u32HeightMm);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_phys_mm_6115(uint32_t u32WidthMm, uint32_t u32HeightMm)
    __attribute__((alias("gj_nested_out_phys_mm_6115")));
