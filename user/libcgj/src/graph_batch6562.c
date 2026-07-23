/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6562: PCIe link speed MT/s stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_speed_mtps_6562(uint32_t gen);
 *     - Map PCIe generation (1..6) to nominal mega-transfers per
 *       second per lane: 2500, 5000, 8000, 16000, 32000, 64000.
 *       Unknown gens return 0. Soft pure-data table only.
 *   uint32_t __gj_pcie_link_speed_mtps_6562  (alias)
 *   __libcgj_batch6562_marker = "libcgj-batch6562"
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

const char __libcgj_batch6562_marker[] = "libcgj-batch6562";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6562_speed_mtps(uint32_t uGen)
{
	switch (uGen) {
	case 1u:
		return 2500u;
	case 2u:
		return 5000u;
	case 3u:
		return 8000u;
	case 4u:
		return 16000u;
	case 5u:
		return 32000u;
	case 6u:
		return 64000u;
	default:
		return 0u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_speed_mtps_6562 - nominal MT/s for a PCIe generation.
 *
 * gen: PCIe generation number (1..6); others -> 0
 *
 * Returns mega-transfers/sec per lane, or 0 if unknown. Soft pure-data
 * table; does not probe hardware. No parent wires.
 */
uint32_t
gj_pcie_link_speed_mtps_6562(uint32_t gen)
{
	(void)NULL;
	return b6562_speed_mtps(gen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_speed_mtps_6562(uint32_t gen)
    __attribute__((alias("gj_pcie_link_speed_mtps_6562")));
