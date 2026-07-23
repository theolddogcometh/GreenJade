/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14138: PipeWire node+link ok soft predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_node_link_ok_u_14138(uint32_t node_id,
 *                                       uint32_t link_id);
 *     - Return 1 if both node_id and link_id are nonzero (soft handles
 *       present), else 0.
 *   uint32_t __gj_pw_node_link_ok_u_14138  (alias)
 *   __libcgj_batch14138_marker = "libcgj-batch14138"
 *
 * Exclusive continuum CREATE-ONLY (14131-14140: audio/pipewire soft
 * stubs — spa_fmt_s16le_id, spa_fmt_f32le_id, channels_ok_u,
 * buf_frames_clamp, latency_nsec_pack, stream_state_norm,
 * context_flags_mask, node_link_ok_u, audio_soft_ready_u,
 * batch_id_14140). Unique gj_pw_node_link_ok_u_14138 surface only; no
 * multi-def. Distinct from gj_pw_node_ok_u_9027 and gj_pw_link_ok_u_9028
 * (single-handle checks). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14138_marker[] = "libcgj-batch14138";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14138_node_link_ok(uint32_t uNodeId, uint32_t uLinkId)
{
	if (uNodeId == 0u) {
		return 0u;
	}
	if (uLinkId == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_node_link_ok_u_14138 - 1 if soft node and link handles are set.
 *
 * node_id / link_id: soft graph handles (0 = invalid / unset).
 * Returns 1 when both are nonzero, else 0. Soft pure-data validity
 * check. No parent wires.
 */
uint32_t
gj_pw_node_link_ok_u_14138(uint32_t uNodeId, uint32_t uLinkId)
{
	(void)NULL;
	return b14138_node_link_ok(uNodeId, uLinkId);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_node_link_ok_u_14138(uint32_t uNodeId, uint32_t uLinkId)
    __attribute__((alias("gj_pw_node_link_ok_u_14138")));
