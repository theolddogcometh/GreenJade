/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6140: PipeWire node-graph continuum id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_graph_batch_id_6140(void);
 *     - Returns the continuum batch id for this PipeWire node-graph
 *       stub wave (always 6140).
 *   uint32_t gj_pw_graph_ready_tag_6140(uint32_t node_ok,
 *                                      uint32_t link_ok);
 *     - Soft graph-ready tag: both lamps nonzero → 0x50574752
 *       fourCC "PWGR"; else 0.
 *   uint32_t __gj_pw_graph_batch_id_6140  (alias)
 *   uint32_t __gj_pw_graph_ready_tag_6140  (alias)
 *   __libcgj_batch6140_marker = "libcgj-batch6140"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs — node_id_norm, port_id_pack, link_ends_pack, media_class_tag,
 * node_state_norm, quantum_clamp, graph_counts_pack, port_dir_mask,
 * link_ready_p, continuum batch_id + ready_tag). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / prior batch_id_*.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6140_marker[] = "libcgj-batch6140";

/* Soft ready fourCC "PWGR" (PipeWire Graph Ready). */
#define B6140_READY_TAG  0x50574752u
#define B6140_BATCH_ID   6140u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6140_batch_id(void)
{
	return B6140_BATCH_ID;
}

static uint32_t
b6140_ready_tag(uint32_t uNodeOk, uint32_t uLinkOk)
{
	if (uNodeOk != 0u && uLinkOk != 0u) {
		return B6140_READY_TAG;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_graph_batch_id_6140 - report this continuum's batch number.
 *
 * Always returns 6140.
 */
uint32_t
gj_pw_graph_batch_id_6140(void)
{
	(void)NULL;
	return b6140_batch_id();
}

/*
 * gj_pw_graph_ready_tag_6140 - soft PipeWire graph-ready fourCC tag.
 *
 * node_ok / link_ok: soft readiness lamps (any nonzero = pass).
 * Returns 0x50574752 ("PWGR") when both pass, else 0.
 * Soft pure-data stub. No parent wires.
 */
uint32_t
gj_pw_graph_ready_tag_6140(uint32_t uNodeOk, uint32_t uLinkOk)
{
	return b6140_ready_tag(uNodeOk, uLinkOk);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pw_graph_batch_id_6140(void)
    __attribute__((alias("gj_pw_graph_batch_id_6140")));

uint32_t __gj_pw_graph_ready_tag_6140(uint32_t uNodeOk, uint32_t uLinkOk)
    __attribute__((alias("gj_pw_graph_ready_tag_6140")));
