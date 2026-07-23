/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4871: pack R,G,B channels into 0x00RRGGBB.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_pack_u(uint32_t r, uint32_t g, uint32_t b);
 *     - Pack unsigned channel words (masked to 8 bits) into a single
 *       0x00RRGGBB pixel word. High byte is always zero.
 *   uint32_t __gj_rgb_pack_u  (alias)
 *   __libcgj_batch4871_marker = "libcgj-batch4871"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880:
 * rgb_pack_u, rgba_pack_u, rgb_r_u, rgb_g_u, rgb_b_u, rgba_a_u,
 * rgb_luma_u, rgb_lerp_u, rgb_is_black_u, batch_id_4880).
 * Distinct from gj_rgb_pack_u32 (batch1681) — unique gj_rgb_pack_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4871_marker[] = "libcgj-batch4871";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4871_ch8(uint32_t u32V)
{
	return u32V & 0xffu;
}

static uint32_t
b4871_pack(uint32_t u32R, uint32_t u32G, uint32_t u32B)
{
	return (b4871_ch8(u32R) << 16) |
	    (b4871_ch8(u32G) << 8) |
	    b4871_ch8(u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_pack_u - pack R,G,B into 0x00RRGGBB.
 *
 * r, g, b: channel values (only the low 8 bits are used)
 *
 * Layout (MSB→LSB): 00 | R | G | B. Matches common XRGB8888 /
 * 0x00RRGGBB desktop pixel words (not BGR). Self-contained; no parent
 * wires.
 */
uint32_t
gj_rgb_pack_u(uint32_t u32R, uint32_t u32G, uint32_t u32B)
{
	(void)NULL;
	return b4871_pack(u32R, u32G, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_pack_u(uint32_t u32R, uint32_t u32G, uint32_t u32B)
    __attribute__((alias("gj_rgb_pack_u")));
