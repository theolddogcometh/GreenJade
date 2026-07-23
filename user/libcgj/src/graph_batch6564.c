/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6564: PCIe link generation ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_gen_ok_6564(uint32_t gen);
 *     - Return 1 if gen is in the supported range 1..6, else 0.
 *       Soft pure-data generation gate for the PCIe continuum.
 *   uint32_t __gj_pcie_link_gen_ok_6564  (alias)
 *   __libcgj_batch6564_marker = "libcgj-batch6564"
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

const char __libcgj_batch6564_marker[] = "libcgj-batch6564";

#define B6564_GEN_MIN  1u
#define B6564_GEN_MAX  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6564_gen_ok(uint32_t uGen)
{
	if (uGen < B6564_GEN_MIN) {
		return 0u;
	}
	if (uGen > B6564_GEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_gen_ok_6564 - supported PCIe generation gate.
 *
 * gen: PCIe generation number
 *
 * Returns 1 when gen is 1..6 inclusive, else 0. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_pcie_link_gen_ok_6564(uint32_t gen)
{
	(void)NULL;
	return b6564_gen_ok(gen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_gen_ok_6564(uint32_t gen)
    __attribute__((alias("gj_pcie_link_gen_ok_6564")));
