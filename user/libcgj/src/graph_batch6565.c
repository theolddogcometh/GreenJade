/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6565: PCIe link max width stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_max_width_6565(void);
 *     - Returns the soft max PCIe link width in lanes covered by this
 *       continuum (16 = x16). Integer-only bound for freestanding
 *       PCIe link speed/width gates.
 *   uint32_t __gj_pcie_link_max_width_6565  (alias)
 *   __libcgj_batch6565_marker = "libcgj-batch6565"
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

const char __libcgj_batch6565_marker[] = "libcgj-batch6565";

/* Soft max PCIe link width in lanes (x16). */
#define B6565_MAX_WIDTH  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6565_max_width(void)
{
	return B6565_MAX_WIDTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_max_width_6565 - soft max PCIe link width (lanes).
 *
 * Always returns 16. Soft bound only; does not probe hardware.
 * No parent wires.
 */
uint32_t
gj_pcie_link_max_width_6565(void)
{
	(void)NULL;
	return b6565_max_width();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_max_width_6565(void)
    __attribute__((alias("gj_pcie_link_max_width_6565")));
