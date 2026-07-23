/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6566: PCIe link encoding ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_enc_ok_6566(uint32_t gen, uint32_t width);
 *     - Return 1 if gen is 1..6 and width is in {1,2,4,8,16}, else 0.
 *       Combined soft pure-data speed/width encoding gate.
 *   uint32_t __gj_pcie_link_enc_ok_6566  (alias)
 *   __libcgj_batch6566_marker = "libcgj-batch6566"
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

const char __libcgj_batch6566_marker[] = "libcgj-batch6566";

#define B6566_GEN_MIN  1u
#define B6566_GEN_MAX  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6566_gen_ok(uint32_t uGen)
{
	if (uGen < B6566_GEN_MIN) {
		return 0u;
	}
	if (uGen > B6566_GEN_MAX) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b6566_width_ok(uint32_t uWidth)
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

static uint32_t
b6566_enc_ok(uint32_t uGen, uint32_t uWidth)
{
	if (b6566_gen_ok(uGen) == 0u) {
		return 0u;
	}
	if (b6566_width_ok(uWidth) == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_enc_ok_6566 - combined gen+width encoding gate.
 *
 * gen:   PCIe generation (must be 1..6)
 * width: link width in lanes (must be 1/2/4/8/16)
 *
 * Returns 1 when both fields are valid, else 0. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_pcie_link_enc_ok_6566(uint32_t gen, uint32_t width)
{
	(void)NULL;
	return b6566_enc_ok(gen, width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_enc_ok_6566(uint32_t gen, uint32_t width)
    __attribute__((alias("gj_pcie_link_enc_ok_6566")));
