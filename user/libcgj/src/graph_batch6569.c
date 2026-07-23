/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6569: PCIe link bandwidth MB/s stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pcie_link_bw_mbs_6569(uint32_t gen, uint32_t width);
 *     - Approximate unidirectional payload bandwidth in MB/s as
 *       (soft MB/s-per-lane table) * width for gen 1..6 and width in
 *       {1,2,4,8,16}. Invalid pairs return 0. Integer-only estimate;
 *       does not probe hardware.
 *   uint32_t __gj_pcie_link_bw_mbs_6569  (alias)
 *   __libcgj_batch6569_marker = "libcgj-batch6569"
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

const char __libcgj_batch6569_marker[] = "libcgj-batch6569";

/* ---- freestanding helpers ---------------------------------------------- */

/* Soft payload MB/s per lane (encoding-adjusted, rounded). */
static uint32_t
b6569_mbs_per_lane(uint32_t uGen)
{
	switch (uGen) {
	case 1u:
		return 250u;   /* Gen1  2.5 GT/s, 8b/10b  */
	case 2u:
		return 500u;   /* Gen2  5.0 GT/s, 8b/10b  */
	case 3u:
		return 985u;   /* Gen3  8.0 GT/s, 128b/130b */
	case 4u:
		return 1969u;  /* Gen4 16.0 GT/s */
	case 5u:
		return 3938u;  /* Gen5 32.0 GT/s */
	case 6u:
		return 7877u;  /* Gen6 64.0 GT/s */
	default:
		return 0u;
	}
}

static uint32_t
b6569_width_ok(uint32_t uWidth)
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
b6569_bw_mbs(uint32_t uGen, uint32_t uWidth)
{
	uint32_t uPerLane;

	if (b6569_width_ok(uWidth) == 0u) {
		return 0u;
	}
	uPerLane = b6569_mbs_per_lane(uGen);
	if (uPerLane == 0u) {
		return 0u;
	}
	/* Max 7877 * 16 = 126032 fits in uint32_t. */
	return uPerLane * uWidth;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pcie_link_bw_mbs_6569 - approximate link payload bandwidth MB/s.
 *
 * gen:   PCIe generation (1..6); others -> 0
 * width: link width in lanes (1/2/4/8/16); others -> 0
 *
 * Returns soft MB/s estimate (per-lane table * width). Pure data only.
 * No parent wires.
 */
uint32_t
gj_pcie_link_bw_mbs_6569(uint32_t gen, uint32_t width)
{
	(void)NULL;
	return b6569_bw_mbs(gen, width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pcie_link_bw_mbs_6569(uint32_t gen, uint32_t width)
    __attribute__((alias("gj_pcie_link_bw_mbs_6569")));
