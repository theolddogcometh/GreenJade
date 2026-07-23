/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5655: keyboard layout id packing.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kbd_layout_pack_5655(uint32_t model, uint32_t layout,
 *                                    uint32_t options);
 *     - Pack XKB-style integer ids for first-boot keyboard page:
 *         bit0..7   = model (0..255)
 *         bit8..15  = layout (0..255)
 *         bit16..23 = options mask (0..255)
 *         bit24..27 = mix nibble
 *   uint32_t gj_kbd_layout_model_5655(uint32_t packed);
 *   uint32_t gj_kbd_layout_id_5655(uint32_t packed);
 *   uint32_t gj_kbd_layout_opts_5655(uint32_t packed);
 *   uint32_t gj_batch_id_5655(void);
 *   __ aliases
 *   __libcgj_batch5655_marker = "libcgj-batch5655"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5655
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5655_marker[] = "libcgj-batch5655";

#define B5655_BATCH_ID 5655u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5655_mix4(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x27d4eb2du;
	u32X ^= u32X >> 15;
	return u32X & 0xfu;
}

static uint32_t
b5655_pack(uint32_t u32Model, uint32_t u32Layout, uint32_t u32Opts)
{
	uint32_t u32P;

	u32Model = u32Model & 0xffu;
	u32Layout = u32Layout & 0xffu;
	u32Opts = u32Opts & 0xffu;

	u32P = u32Model | (u32Layout << 8) | (u32Opts << 16);
	return u32P | (b5655_mix4(u32P ^ B5655_BATCH_ID) << 24);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_kbd_layout_pack_5655(uint32_t model, uint32_t layout, uint32_t options)
{
	(void)NULL;
	return b5655_pack(model, layout, options);
}

uint32_t
gj_kbd_layout_model_5655(uint32_t packed)
{
	return packed & 0xffu;
}

uint32_t
gj_kbd_layout_id_5655(uint32_t packed)
{
	return (packed >> 8) & 0xffu;
}

uint32_t
gj_kbd_layout_opts_5655(uint32_t packed)
{
	return (packed >> 16) & 0xffu;
}

uint32_t
gj_batch_id_5655(void)
{
	return B5655_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_kbd_layout_pack_5655(uint32_t model, uint32_t layout,
    uint32_t options)
    __attribute__((alias("gj_kbd_layout_pack_5655")));

uint32_t __gj_kbd_layout_model_5655(uint32_t packed)
    __attribute__((alias("gj_kbd_layout_model_5655")));

uint32_t __gj_kbd_layout_id_5655(uint32_t packed)
    __attribute__((alias("gj_kbd_layout_id_5655")));

uint32_t __gj_kbd_layout_opts_5655(uint32_t packed)
    __attribute__((alias("gj_kbd_layout_opts_5655")));

uint32_t __gj_batch_id_5655(void)
    __attribute__((alias("gj_batch_id_5655")));
