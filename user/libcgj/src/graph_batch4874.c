/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4874: extract the green channel from 0xAARRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_g_u(uint32_t c);
 *     - Return the G channel of packed color c as 0..255
 *       ((c >> 8) & 0xff). Works for both 0x00RRGGBB and 0xAARRGGBB.
 *   uint32_t __gj_rgb_g_u  (alias)
 *   __libcgj_batch4874_marker = "libcgj-batch4874"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_rgb_unpack_u32 (batch1682) and gj_rgb_r_u (batch4873) —
 * unique gj_rgb_g_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4874_marker[] = "libcgj-batch4874";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4874_g(uint32_t u32C)
{
	return (u32C >> 8) & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_g_u - extract green channel from a packed RGB/RGBA word.
 *
 * c: packed color (0xAARRGGBB or 0x00RRGGBB)
 *
 * Returns G in 0..255. Self-contained; no parent wires.
 */
uint32_t
gj_rgb_g_u(uint32_t u32C)
{
	(void)NULL;
	return b4874_g(u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_g_u(uint32_t u32C)
    __attribute__((alias("gj_rgb_g_u")));
