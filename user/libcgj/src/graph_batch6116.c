/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6116: nested output stub tag magic (_6116).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_out_tag_6116(uint32_t flags);
 *     - Pure stub for a gamescope nested-output presence tag.
 *       Returns (0x4E4F << 16) | (flags & 0xFFFF) ("NO" high half).
 *   uint32_t __gj_nested_out_tag_6116  (alias)
 *   __libcgj_batch6116_marker = "libcgj-batch6116"
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

const char __libcgj_batch6116_marker[] = "libcgj-batch6116";

/* "NO" high half — nested output soft tag. */
#define B6116_MAGIC_HI   0x4E4F0000u
#define B6116_FLAGS_MASK 0x0000FFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6116_tag(uint32_t u32Flags)
{
	return B6116_MAGIC_HI | (u32Flags & B6116_FLAGS_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_out_tag_6116 - gamescope nested output presence tag.
 *
 * flags: soft capability bits (low 16 retained)
 *
 * Returns packed magic with "NO" in the high half. Pure integer stub;
 * no Wayland registry bind. No parent wires.
 */
uint32_t
gj_nested_out_tag_6116(uint32_t u32Flags)
{
	(void)NULL;
	return b6116_tag(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_out_tag_6116(uint32_t u32Flags)
    __attribute__((alias("gj_nested_out_tag_6116")));
