/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6133: PipeWire link endpoint pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_link_ends_pack_6133(uint32_t src_node,
 *                                      uint32_t dst_node);
 *     - Pack source and destination node ids into a soft link-ends
 *       stub key: [31:16] src lo16 | [15:0] dst lo16.
 *   uint32_t __gj_pw_link_ends_pack_6133  (alias)
 *   __libcgj_batch6133_marker = "libcgj-batch6133"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_link_ends_pack_6133 surface only; no multi-def.
 * Distinct from port pack (batch6132). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6133_marker[] = "libcgj-batch6133";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6133_link_ends_pack(uint32_t uSrc, uint32_t uDst)
{
	return ((uSrc & 0xFFFFu) << 16) | (uDst & 0xFFFFu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_link_ends_pack_6133 - pack PipeWire-shaped link endpoints.
 *
 * src_node / dst_node: soft graph node ids (low 16 bits each).
 * Returns packed 32-bit ends key. Soft pure-data stub. No parent wires.
 */
uint32_t
gj_pw_link_ends_pack_6133(uint32_t uSrcNode, uint32_t uDstNode)
{
	(void)NULL;
	return b6133_link_ends_pack(uSrcNode, uDstNode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_link_ends_pack_6133(uint32_t uSrcNode, uint32_t uDstNode)
    __attribute__((alias("gj_pw_link_ends_pack_6133")));
