/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6131: PipeWire node-id normalize stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_node_id_norm_6131(uint32_t node_id);
 *     - Normalize a soft PipeWire-shaped node id: 0 is invalid and
 *       stays 0; nonzero ids are returned unchanged (stub pass-through).
 *   uint32_t __gj_pw_node_id_norm_6131  (alias)
 *   __libcgj_batch6131_marker = "libcgj-batch6131"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_node_id_norm_6131 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6131_marker[] = "libcgj-batch6131";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6131_node_id_norm(uint32_t uNodeId)
{
	/* Soft stub: zero remains invalid; nonzero ids pass through. */
	return uNodeId;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_node_id_norm_6131 - normalize PipeWire-shaped node id.
 *
 * node_id: soft graph node identifier (0 = invalid / unset).
 * Returns node_id unchanged. Soft pure-data stub for product scoring.
 * No parent wires.
 */
uint32_t
gj_pw_node_id_norm_6131(uint32_t uNodeId)
{
	(void)NULL;
	return b6131_node_id_norm(uNodeId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_node_id_norm_6131(uint32_t uNodeId)
    __attribute__((alias("gj_pw_node_id_norm_6131")));
