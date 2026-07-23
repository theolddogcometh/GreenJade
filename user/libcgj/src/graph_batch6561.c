/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6561: PCIe link max generation stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_max_gen_6561(void);
 *     - Returns the soft max PCIe generation number supported by this
 *       continuum (6 = Gen6). Integer-only bound for freestanding
 *       PCIe link speed/width gates.
 *   uint32_t __gj_pcie_link_max_gen_6561  (alias)
 *   __libcgj_batch6561_marker = "libcgj-batch6561"
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

const char __libcgj_batch6561_marker[] = "libcgj-batch6561";

/* Soft max PCIe generation covered by this continuum (Gen6). */
#define B6561_MAX_GEN  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6561_max_gen(void)
{
	return B6561_MAX_GEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_max_gen_6561 - soft max PCIe generation number.
 *
 * Always returns 6. Soft bound only; does not probe hardware.
 * No parent wires.
 */
uint32_t
gj_pcie_link_max_gen_6561(void)
{
	(void)NULL;
	return b6561_max_gen();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_max_gen_6561(void)
    __attribute__((alias("gj_pcie_link_max_gen_6561")));
