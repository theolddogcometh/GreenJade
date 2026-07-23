/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5652: first-boot wizard phase packing.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fbwiz_phase_pack_5652(uint32_t phase, uint32_t sub,
 *                                     uint32_t done_mask);
 *     - Pack phase state:
 *         bit0..3   = phase clamped 0..15 (welcome/locale/kbd/net/...)
 *         bit4..7   = sub-step clamped 0..15
 *         bit8..23  = done_mask low 16 bits
 *         bit24..27 = simple mix nibble
 *   uint32_t gj_fbwiz_phase_get_5652(uint32_t packed);
 *   uint32_t gj_fbwiz_phase_sub_5652(uint32_t packed);
 *   uint32_t gj_fbwiz_phase_done_5652(uint32_t packed);
 *   uint32_t gj_batch_id_5652(void);
 *   __ aliases
 *   __libcgj_batch5652_marker = "libcgj-batch5652"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5652
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5652_marker[] = "libcgj-batch5652";

#define B5652_BATCH_ID 5652u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5652_clamp4(uint32_t u32V)
{
	if (u32V > 15u) {
		return 15u;
	}
	return u32V;
}

static uint32_t
b5652_mix4(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x7feb352du;
	u32X ^= u32X >> 15;
	return u32X & 0xfu;
}

static uint32_t
b5652_pack(uint32_t u32Phase, uint32_t u32Sub, uint32_t u32Done)
{
	uint32_t u32P;

	u32Phase = b5652_clamp4(u32Phase);
	u32Sub = b5652_clamp4(u32Sub);
	u32Done = u32Done & 0xffffu;

	u32P = u32Phase | (u32Sub << 4) | (u32Done << 8);
	return u32P | (b5652_mix4(u32P ^ B5652_BATCH_ID) << 24);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_fbwiz_phase_pack_5652(uint32_t phase, uint32_t sub, uint32_t done_mask)
{
	(void)NULL;
	return b5652_pack(phase, sub, done_mask);
}

uint32_t
gj_fbwiz_phase_get_5652(uint32_t packed)
{
	return packed & 0xfu;
}

uint32_t
gj_fbwiz_phase_sub_5652(uint32_t packed)
{
	return (packed >> 4) & 0xfu;
}

uint32_t
gj_fbwiz_phase_done_5652(uint32_t packed)
{
	return (packed >> 8) & 0xffffu;
}

uint32_t
gj_batch_id_5652(void)
{
	return B5652_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fbwiz_phase_pack_5652(uint32_t phase, uint32_t sub,
    uint32_t done_mask)
    __attribute__((alias("gj_fbwiz_phase_pack_5652")));

uint32_t __gj_fbwiz_phase_get_5652(uint32_t packed)
    __attribute__((alias("gj_fbwiz_phase_get_5652")));

uint32_t __gj_fbwiz_phase_sub_5652(uint32_t packed)
    __attribute__((alias("gj_fbwiz_phase_sub_5652")));

uint32_t __gj_fbwiz_phase_done_5652(uint32_t packed)
    __attribute__((alias("gj_fbwiz_phase_done_5652")));

uint32_t __gj_batch_id_5652(void)
    __attribute__((alias("gj_batch_id_5652")));
