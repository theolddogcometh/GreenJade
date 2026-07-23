/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6137: PipeWire graph node/port count pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_graph_counts_pack_6137(uint32_t node_count,
 *                                         uint32_t port_count);
 *     - Pack soft graph inventory counts into a 32-bit stub:
 *         [31:16] node_count lo16 | [15:0] port_count lo16
 *       Each count is saturated to 0xFFFF before packing.
 *   uint32_t __gj_pw_graph_counts_pack_6137  (alias)
 *   __libcgj_batch6137_marker = "libcgj-batch6137"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_graph_counts_pack_6137 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6137_marker[] = "libcgj-batch6137";

#define B6137_COUNT_MAX  0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6137_sat16(uint32_t uV)
{
	if (uV > B6137_COUNT_MAX) {
		return B6137_COUNT_MAX;
	}
	return uV;
}

static uint32_t
b6137_counts_pack(uint32_t uNodes, uint32_t uPorts)
{
	return (b6137_sat16(uNodes) << 16) | b6137_sat16(uPorts);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_graph_counts_pack_6137 - pack soft node/port inventory counts.
 *
 * node_count / port_count: soft graph inventory sizes.
 * Returns packed 32-bit counts key (each side sat to 16 bits).
 * Soft pure-data stub. No parent wires.
 */
uint32_t
gj_pw_graph_counts_pack_6137(uint32_t uNodeCount, uint32_t uPortCount)
{
	(void)NULL;
	return b6137_counts_pack(uNodeCount, uPortCount);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_graph_counts_pack_6137(uint32_t uNodeCount,
    uint32_t uPortCount)
    __attribute__((alias("gj_pw_graph_counts_pack_6137")));
