/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6120(void);
 *     - Returns the compile-time graph batch number for this TU (6120).
 *   uint32_t __gj_batch_id_6120  (alias)
 *   __libcgj_batch6120_marker = "libcgj-batch6120"
 *
 * Exclusive continuum CREATE-ONLY (6111-6120: gamescope nested output —
 * nested_out_mode_pack_6111, nested_out_scale_6112, nested_out_xform_6113,
 * nested_out_refresh_hz_6114, nested_out_phys_mm_6115, nested_out_tag_6116,
 * nested_out_pos_pack_6117, nested_out_bpp_6118, nested_out_subpixel_6119,
 * batch_id_6120). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6120_marker[] = "libcgj-batch6120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6120_id(void)
{
	return 6120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6120 - report this TU's graph batch number.
 *
 * Always returns 6120. Closes the 6111-6120 gamescope nested output wave.
 */
uint32_t
gj_batch_id_6120(void)
{
	(void)NULL;
	return b6120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6120(void)
    __attribute__((alias("gj_batch_id_6120")));
