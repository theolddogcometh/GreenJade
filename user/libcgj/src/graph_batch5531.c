/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5531: gamescope compositor stub magic (_5531).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gscope_comp_stub_5531(uint32_t flags);
 *     - Pure stub for a gamescope-style compositor presence probe.
 *       Returns a packed magic (0x4753 << 16) | (flags & 0xFFFF)
 *       ("GS" high half). flags is soft-masked; no Wayland/X11 I/O.
 *   uint32_t __gj_gscope_comp_stub_5531  (alias)
 *   __libcgj_batch5531_marker = "libcgj-batch5531"
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

const char __libcgj_batch5531_marker[] = "libcgj-batch5531";

/* "GS" high half — soft gamescope compositor stub tag. */
#define B5531_MAGIC_HI  0x47530000u
#define B5531_FLAGS_MASK 0x0000FFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5531_stub(uint32_t u32Flags)
{
	return B5531_MAGIC_HI | (u32Flags & B5531_FLAGS_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gscope_comp_stub_5531 - gamescope compositor stub presence tag.
 *
 * flags: soft capability bits (low 16 retained; high bits ignored)
 *
 * Returns packed magic with "GS" in the high half and masked flags in
 * the low half. Pure integer stub; no compositor connection. No parent
 * wires.
 */
uint32_t
gj_gscope_comp_stub_5531(uint32_t u32Flags)
{
	(void)NULL;
	return b5531_stub(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gscope_comp_stub_5531(uint32_t u32Flags)
    __attribute__((alias("gj_gscope_comp_stub_5531")));
