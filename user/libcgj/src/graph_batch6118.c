/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6118: nested output bits-per-pixel soft (_6118).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_bpp_6118(uint32_t bpp);
 *     - Soft-pick nested output colour depth. Accepted: 16, 24, 30, 32.
 *       Any other value → 32 (default ARGB8888-style depth).
 *   uint32_t __gj_nested_out_bpp_6118  (alias)
 *   __libcgj_batch6118_marker = "libcgj-batch6118"
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

const char __libcgj_batch6118_marker[] = "libcgj-batch6118";

#define B6118_BPP_DEFAULT  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6118_pick(uint32_t u32Bpp)
{
	if (u32Bpp == 16u || u32Bpp == 24u || u32Bpp == 30u || u32Bpp == 32u) {
		return u32Bpp;
	}
	return B6118_BPP_DEFAULT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_bpp_6118 - soft nested output bits-per-pixel pick.
 *
 * bpp: requested colour depth
 *
 * Returns 16, 24, 30, or 32; else 32. Pure table stub; no GBM/DRM
 * format enum. No parent wires.
 */
uint32_t
gj_nested_out_bpp_6118(uint32_t u32Bpp)
{
	(void)NULL;
	return b6118_pick(u32Bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_bpp_6118(uint32_t u32Bpp)
    __attribute__((alias("gj_nested_out_bpp_6118")));
