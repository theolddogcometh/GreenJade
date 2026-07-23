/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6567: PCIe link pack (gen|width).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_pack_6567(uint32_t gen, uint32_t width);
 *     - Pack soft link fields into one summary word:
 *         bits  0..7  = gen   clamped to 0..0xff
 *         bits  8..15 = width clamped to 0..0xff
 *         bits 16..31 = 0
 *       Compact probe tag for the PCIe link continuum.
 *   uint32_t __gj_pcie_link_pack_6567  (alias)
 *   __libcgj_batch6567_marker = "libcgj-batch6567"
 *
 * Exclusive continuum CREATE-ONLY (6561-6570: PCIe link speed/width
 * stubs — max_gen_6561, speed_mtps_6562, width_ok_6563, gen_ok_6564,
 * max_width_6565, enc_ok_6566, pack_6567, score_6568, bw_mbs_6569,
 * batch_id / wave_ready_6570). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6567_marker[] = "libcgj-batch6567";

#define B6567_U8_MAX  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6567_clamp8(uint32_t u32V)
{
	if (u32V > B6567_U8_MAX) {
		return B6567_U8_MAX;
	}
	return u32V;
}

static uint32_t
b6567_pack(uint32_t u32Gen, uint32_t u32Width)
{
	return b6567_clamp8(u32Gen) | (b6567_clamp8(u32Width) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_pack_6567 - pack generation + width into one word.
 *
 * gen:   PCIe generation (clamped to 8 bits)
 * width: link width in lanes (clamped to 8 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_pcie_link_pack_6567(uint32_t gen, uint32_t width)
{
	(void)NULL;
	return b6567_pack(gen, width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_pack_6567(uint32_t gen, uint32_t width)
    __attribute__((alias("gj_pcie_link_pack_6567")));
