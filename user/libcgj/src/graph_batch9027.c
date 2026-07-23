/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9027: PipeWire node-id ok soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_node_ok_u_9027(uint32_t node_id);
 *     - Return 1 if node_id is nonzero (valid soft node handle), else 0.
 *   uint32_t __gj_pw_node_ok_u_9027  (alias)
 *   __libcgj_batch9027_marker = "libcgj-batch9027"
 *
 * Exclusive continuum CREATE-ONLY (9021-9030: pipewire/pulse soft id
 * stubs — stream_play_id, stream_cap_id, format_s16_id, format_f32_id,
 * rate_ok_u, pulse_ready_u, node_ok_u, link_ok_u, pipewire_ready_u,
 * batch_id_9030). Unique gj_pw_node_ok_u_9027 surface only; no
 * multi-def. Distinct from gj_pw_node_id_norm_6131 and
 * gj_pw_graph_ready_tag_6140 node_ok lamp. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9027_marker[] = "libcgj-batch9027";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9027_pw_node_ok(uint32_t u32NodeId)
{
	/* Soft stub: zero remains invalid / unset; nonzero is ok. */
	return (u32NodeId != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_node_ok_u_9027 - 1 if soft PipeWire-shaped node id is set.
 *
 * node_id: soft graph node identifier (0 = invalid / unset).
 * Returns 1 for nonzero ids, else 0. Soft pure-data validity check.
 * No parent wires.
 */
uint32_t
gj_pw_node_ok_u_9027(uint32_t u32NodeId)
{
	(void)NULL;
	return b9027_pw_node_ok(u32NodeId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_node_ok_u_9027(uint32_t u32NodeId)
    __attribute__((alias("gj_pw_node_ok_u_9027")));
