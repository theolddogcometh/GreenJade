/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4878: per-channel RGB lerp of packed colors.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rgb_lerp_u(uint32_t a, uint32_t b, uint32_t t);
 *     - Linear interpolate packed RGB words a→b with weight t in 0..255.
 *       t==0 → a RGB; t==255 → b RGB. Channels mixed independently;
 *       result high byte is zero (0x00RRGGBB). t is clamped to 0..255.
 *   uint32_t __gj_rgb_lerp_u  (alias)
 *   __libcgj_batch4878_marker = "libcgj-batch4878"
 *
 * Exclusive continuum CREATE-ONLY color pack wave (4871-4880). Distinct
 * from gj_u32_lerp_q16 (batch1478) and gj_lerp_i32 (batch959) — unique
 * gj_rgb_lerp_u packed-RGB surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4878_marker[] = "libcgj-batch4878";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4878_ch8(uint32_t u32V)
{
	return u32V & 0xffu;
}

/*
 * Mix one 8-bit channel: (a*(255-t) + b*t) / 255 with t in 0..255.
 * Exact endpoints: t==0 → a, t==255 → b.
 */
static uint32_t
b4878_mix(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	return (u32A * (255u - u32T) + u32B * u32T) / 255u;
}

static uint32_t
b4878_lerp(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	uint32_t u32Ra;
	uint32_t u32Ga;
	uint32_t u32Ba;
	uint32_t u32Rb;
	uint32_t u32Gb;
	uint32_t u32Bb;
	uint32_t u32Ro;
	uint32_t u32Go;
	uint32_t u32Bo;

	if (u32T > 255u) {
		u32T = 255u;
	}
	u32Ra = (u32A >> 16) & 0xffu;
	u32Ga = (u32A >> 8) & 0xffu;
	u32Ba = u32A & 0xffu;
	u32Rb = (u32B >> 16) & 0xffu;
	u32Gb = (u32B >> 8) & 0xffu;
	u32Bb = u32B & 0xffu;
	u32Ro = b4878_mix(u32Ra, u32Rb, u32T);
	u32Go = b4878_mix(u32Ga, u32Gb, u32T);
	u32Bo = b4878_mix(u32Ba, u32Bb, u32T);
	return (b4878_ch8(u32Ro) << 16) |
	    (b4878_ch8(u32Go) << 8) |
	    b4878_ch8(u32Bo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rgb_lerp_u - per-channel lerp of packed RGB words with t in 0..255.
 *
 * a, b: packed colors (alpha ignored; result alpha is 0)
 * t: blend weight 0..255 (values above 255 clamp to 255)
 *
 * Returns 0x00RRGGBB. t==0 yields a's RGB; t==255 yields b's RGB.
 * Self-contained; no parent wires.
 */
uint32_t
gj_rgb_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	(void)NULL;
	return b4878_lerp(u32A, u32B, u32T);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rgb_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32T)
    __attribute__((alias("gj_rgb_lerp_u")));
