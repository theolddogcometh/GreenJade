/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6132: PipeWire port-id pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_port_id_pack_6132(uint32_t node_id,
 *                                    uint32_t direction,
 *                                    uint32_t port_index);
 *     - Pack a soft port identity from node, direction (0=in, 1=out),
 *       and port index into a 32-bit stub key:
 *         [31:16] node_id lo16 | [15:8] direction&1 | [7:0] index&0xff
 *   uint32_t __gj_pw_port_id_pack_6132  (alias)
 *   __libcgj_batch6132_marker = "libcgj-batch6132"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_port_id_pack_6132 surface only; no multi-def.
 * Distinct from gj_pw_node_id_norm_6131. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6132_marker[] = "libcgj-batch6132";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6132_port_id_pack(uint32_t uNodeId, uint32_t uDir, uint32_t uIdx)
{
	uint32_t uN;
	uint32_t uD;
	uint32_t uI;

	uN = uNodeId & 0xFFFFu;
	uD = uDir & 1u;
	uI = uIdx & 0xFFu;
	return (uN << 16) | (uD << 8) | uI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_port_id_pack_6132 - pack PipeWire-shaped port id stub key.
 *
 * node_id: soft node identifier (low 16 bits used).
 * direction: 0 = input, 1 = output (other bits ignored).
 * port_index: soft port ordinal (low 8 bits used).
 * Returns packed 32-bit stub key. Soft pure-data only. No parent wires.
 */
uint32_t
gj_pw_port_id_pack_6132(uint32_t uNodeId, uint32_t uDirection,
    uint32_t uPortIndex)
{
	(void)NULL;
	return b6132_port_id_pack(uNodeId, uDirection, uPortIndex);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_port_id_pack_6132(uint32_t uNodeId, uint32_t uDirection,
    uint32_t uPortIndex)
    __attribute__((alias("gj_pw_port_id_pack_6132")));
