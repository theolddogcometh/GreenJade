/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5552: controller layout profile packing.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ctrl_layout_pack_5552(uint32_t face_mask, uint32_t sticks,
 *                                     uint32_t pads);
 *     - Pack a layout profile id from feature bits:
 *         bit0..7  = face_mask (which face buttons present)
 *         bit8..11 = sticks clamped 0..3
 *         bit12..13= pads clamped 0..3 (trackpads / touch surfaces)
 *         bit16..  = simple FNV-ish mix nibble for uniqueness
 *   uint32_t gj_ctrl_layout_face_5552(uint32_t packed);
 *     - Extract face_mask (low 8 bits).
 *   uint32_t gj_ctrl_layout_sticks_5552(uint32_t packed);
 *   uint32_t gj_ctrl_layout_pads_5552(uint32_t packed);
 *   uint32_t gj_batch_id_5552(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5552_marker = "libcgj-batch5552"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5552
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5552_marker[] = "libcgj-batch5552";

#define B5552_BATCH_ID 5552u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5552_clamp_u(uint32_t u32V, uint32_t u32MaxIncl)
{
	if (u32V > u32MaxIncl) {
		return u32MaxIncl;
	}
	return u32V;
}

/* Small non-crypto mix for layout fingerprint nibble. */
static uint32_t
b5552_mix4(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x7feb352du;
	u32X ^= u32X >> 15;
	return u32X & 0xfu;
}

static uint32_t
b5552_pack(uint32_t u32Face, uint32_t u32Sticks, uint32_t u32Pads)
{
	uint32_t u32P;
	uint32_t u32Mix;

	u32Face = u32Face & 0xffu;
	u32Sticks = b5552_clamp_u(u32Sticks, 3u);
	u32Pads = b5552_clamp_u(u32Pads, 3u);

	u32P = u32Face
	    | (u32Sticks << 8)
	    | (u32Pads << 12);
	u32Mix = b5552_mix4(u32P ^ B5552_BATCH_ID);
	return u32P | (u32Mix << 16);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_ctrl_layout_pack_5552(uint32_t face_mask, uint32_t sticks, uint32_t pads)
{
	(void)NULL;
	return b5552_pack(face_mask, sticks, pads);
}

uint32_t
gj_ctrl_layout_face_5552(uint32_t packed)
{
	return packed & 0xffu;
}

uint32_t
gj_ctrl_layout_sticks_5552(uint32_t packed)
{
	return (packed >> 8) & 0xfu;
}

uint32_t
gj_ctrl_layout_pads_5552(uint32_t packed)
{
	return (packed >> 12) & 0x3u;
}

uint32_t
gj_batch_id_5552(void)
{
	return B5552_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_ctrl_layout_pack_5552(uint32_t face_mask, uint32_t sticks,
    uint32_t pads)
    __attribute__((alias("gj_ctrl_layout_pack_5552")));

uint32_t __gj_ctrl_layout_face_5552(uint32_t packed)
    __attribute__((alias("gj_ctrl_layout_face_5552")));

uint32_t __gj_ctrl_layout_sticks_5552(uint32_t packed)
    __attribute__((alias("gj_ctrl_layout_sticks_5552")));

uint32_t __gj_ctrl_layout_pads_5552(uint32_t packed)
    __attribute__((alias("gj_ctrl_layout_pads_5552")));

uint32_t __gj_batch_id_5552(void)
    __attribute__((alias("gj_batch_id_5552")));
