/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6112: nested output scale factor soft (_6112).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_scale_6112(uint32_t scale_x100);
 *     - Clamp a nested-output scale expressed in hundredths (100 = 1.0x)
 *       to [100, 400] (1.0x..4.0x). Below 100 → 100; above 400 → 400.
 *   uint32_t __gj_nested_out_scale_6112  (alias)
 *   __libcgj_batch6112_marker = "libcgj-batch6112"
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

const char __libcgj_batch6112_marker[] = "libcgj-batch6112";

#define B6112_SCALE_MIN  100u
#define B6112_SCALE_MAX  400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6112_clamp(uint32_t u32Scale)
{
	if (u32Scale < B6112_SCALE_MIN) {
		return B6112_SCALE_MIN;
	}
	if (u32Scale > B6112_SCALE_MAX) {
		return B6112_SCALE_MAX;
	}
	return u32Scale;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_scale_6112 - clamp nested output scale hundredths.
 *
 * scale_x100: requested scale * 100 (100 = 1.0x, 200 = 2.0x)
 *
 * Returns value in [100, 400]. Pure integer; no fractional compositor
 * protocol. No parent wires.
 */
uint32_t
gj_nested_out_scale_6112(uint32_t u32ScaleX100)
{
	(void)NULL;
	return b6112_clamp(u32ScaleX100);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_scale_6112(uint32_t u32ScaleX100)
    __attribute__((alias("gj_nested_out_scale_6112")));
