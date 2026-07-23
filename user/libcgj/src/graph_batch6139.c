/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6139: PipeWire link-ready predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_link_ready_p_6139(uint32_t src_node,
 *                                    uint32_t dst_node,
 *                                    uint32_t src_state,
 *                                    uint32_t dst_state);
 *     - Soft link-ready predicate: both node ids nonzero and both
 *       states equal to 4 (running). Returns 1 if ready, else 0.
 *   uint32_t __gj_pw_link_ready_p_6139  (alias)
 *   __libcgj_batch6139_marker = "libcgj-batch6139"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_link_ready_p_6139 surface only; no multi-def.
 * Aligns with state code 4 = running from batch6135. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6139_marker[] = "libcgj-batch6139";

#define B6139_STATE_RUNNING  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6139_link_ready_p(uint32_t uSrc, uint32_t uDst, uint32_t uSrcSt,
    uint32_t uDstSt)
{
	if (uSrc == 0u || uDst == 0u) {
		return 0u;
	}
	if (uSrcSt != B6139_STATE_RUNNING) {
		return 0u;
	}
	if (uDstSt != B6139_STATE_RUNNING) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_link_ready_p_6139 - soft PipeWire link-ready predicate.
 *
 * src_node / dst_node: soft endpoint node ids (must be nonzero).
 * src_state / dst_state: soft node states (must be 4 = running).
 * Returns 1 if all checks pass, else 0. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_link_ready_p_6139(uint32_t uSrcNode, uint32_t uDstNode,
    uint32_t uSrcState, uint32_t uDstState)
{
	(void)NULL;
	return b6139_link_ready_p(uSrcNode, uDstNode, uSrcState, uDstState);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_link_ready_p_6139(uint32_t uSrcNode, uint32_t uDstNode,
    uint32_t uSrcState, uint32_t uDstState)
    __attribute__((alias("gj_pw_link_ready_p_6139")));
