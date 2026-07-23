/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6113: nested output transform normalize (_6113).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_xform_6113(uint32_t transform);
 *     - Normalize a wl_output-style transform code to {0,1,2,3}
 *       (0/90/180/270°). Values outside 0..7 collapse to 0; 4..7 map
 *       to flipped equivalents 0..3 (soft: transform & 3).
 *   uint32_t __gj_nested_out_xform_6113  (alias)
 *   __libcgj_batch6113_marker = "libcgj-batch6113"
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

const char __libcgj_batch6113_marker[] = "libcgj-batch6113";

#define B6113_XFORM_MAX  7u
#define B6113_ROT_MASK   3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6113_norm(uint32_t u32Xform)
{
	if (u32Xform > B6113_XFORM_MAX) {
		return 0u;
	}
	return u32Xform & B6113_ROT_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_xform_6113 - normalize nested output transform to 0..3.
 *
 * transform: soft wl_output transform code (0..7 accepted)
 *
 * Returns rotation band 0..3; out-of-range → 0. Pure integer stub; no
 * buffer reorientation. No parent wires.
 */
uint32_t
gj_nested_out_xform_6113(uint32_t u32Transform)
{
	(void)NULL;
	return b6113_norm(u32Transform);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_xform_6113(uint32_t u32Transform)
    __attribute__((alias("gj_nested_out_xform_6113")));
