/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9230: websocket soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9230(void);
 *     - Returns the compile-time graph batch number for this TU (9230).
 *   uint32_t __gj_batch_id_9230  (alias)
 *   __libcgj_batch9230_marker = "libcgj-batch9230"
 *
 * Exclusive continuum CREATE-ONLY (9221-9230: websocket soft id
 * stubs — ws_opcode_text_id_9221, ws_opcode_bin_id_9222,
 * ws_opcode_close_id_9223, ws_opcode_ping_id_9224,
 * ws_opcode_pong_id_9225, ws_fin_id_9226, ws_mask_ok_u_9227,
 * ws_frame_ok_u_9228, ws_ready_u_9229, batch_id_9230).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No WebSocket framing implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9230_marker[] = "libcgj-batch9230";

#define B9230_BATCH_ID  9230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9230_id(void)
{
	return B9230_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9230 - report this TU's graph batch number.
 *
 * Always returns 9230. Link-time presence tags the exclusive
 * websocket soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9230(void)
{
	(void)NULL;
	return b9230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9230(void)
    __attribute__((alias("gj_batch_id_9230")));
