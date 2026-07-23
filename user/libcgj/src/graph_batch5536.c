/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5536: gamescope layer Z-order pack (_5536).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gscope_layer_z_5536(uint32_t base, uint32_t slot);
 *     - Pack a compositor layer Z key: (base << 8) | (slot & 0xFF).
 *       base is soft-masked to 24 bits before shift so the result always
 *       fits in uint32_t. slot low byte selects sub-order within base.
 *   uint32_t __gj_gscope_layer_z_5536  (alias)
 *   __libcgj_batch5536_marker = "libcgj-batch5536"
 *
 * Exclusive continuum CREATE-ONLY (5531-5540: gamescope compositor —
 * gscope_comp_stub_5531, nested_refresh_hz_5532, fsr_sharpness_5533,
 * integer_scale_5534, deck_ui_chrome_5535, gscope_layer_z_5536,
 * nested_refresh_us_5537, fsr_sharp_q8_5538, integer_scale_fit_5539,
 * batch_id_5540). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5536_marker[] = "libcgj-batch5536";

#define B5536_BASE_MASK  0x00FFFFFFu
#define B5536_SLOT_MASK  0x000000FFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5536_pack(uint32_t u32Base, uint32_t u32Slot)
{
	return ((u32Base & B5536_BASE_MASK) << 8) | (u32Slot & B5536_SLOT_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gscope_layer_z_5536 - pack gamescope-style layer Z key.
 *
 * base: major layer band (24-bit soft mask)
 * slot: sub-order within band (low 8 bits)
 *
 * Returns (base24 << 8) | slot8. Pure integer pack; no scene graph.
 * No parent wires.
 */
uint32_t
gj_gscope_layer_z_5536(uint32_t u32Base, uint32_t u32Slot)
{
	(void)NULL;
	return b5536_pack(u32Base, u32Slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gscope_layer_z_5536(uint32_t u32Base, uint32_t u32Slot)
    __attribute__((alias("gj_gscope_layer_z_5536")));
