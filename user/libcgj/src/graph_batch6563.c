/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6563: PCIe link width ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_width_ok_6563(uint32_t width);
 *     - Return 1 if width is a standard PCIe lane count in
 *       {1, 2, 4, 8, 16}, else 0. Soft pure-data width gate.
 *   uint32_t __gj_pcie_link_width_ok_6563  (alias)
 *   __libcgj_batch6563_marker = "libcgj-batch6563"
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

const char __libcgj_batch6563_marker[] = "libcgj-batch6563";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6563_width_ok(uint32_t uWidth)
{
	switch (uWidth) {
	case 1u:
	case 2u:
	case 4u:
	case 8u:
	case 16u:
		return 1u;
	default:
		return 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_width_ok_6563 - standard PCIe lane-count gate.
 *
 * width: requested link width in lanes
 *
 * Returns 1 when width is 1, 2, 4, 8, or 16; else 0. Soft pure-data
 * only. No parent wires.
 */
uint32_t
gj_pcie_link_width_ok_6563(uint32_t width)
{
	(void)NULL;
	return b6563_width_ok(width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_width_ok_6563(uint32_t width)
    __attribute__((alias("gj_pcie_link_width_ok_6563")));
