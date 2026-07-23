/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9221: soft WebSocket text opcode catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ws_opcode_text_id_9221(void);
 *     - Returns 0x1 (RFC 6455 opcode TEXT soft catalog value). Pure-data
 *       opcode id stub; does not parse frames or open sockets.
 *   uint32_t __gj_ws_opcode_text_id_9221  (alias)
 *   __libcgj_batch9221_marker = "libcgj-batch9221"
 *
 * Exclusive continuum CREATE-ONLY (9221-9230: websocket soft id
 * stubs — ws_opcode_text_id_9221, ws_opcode_bin_id_9222,
 * ws_opcode_close_id_9223, ws_opcode_ping_id_9224,
 * ws_opcode_pong_id_9225, ws_fin_id_9226, ws_mask_ok_u_9227,
 * ws_frame_ok_u_9228, ws_ready_u_9229, batch_id_9230).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No WebSocket framing implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9221_marker[] = "libcgj-batch9221";

/* Soft RFC 6455 opcode TEXT (0x1). */
#define B9221_WS_OPCODE_TEXT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9221_opcode_text_id(void)
{
	return B9221_WS_OPCODE_TEXT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_opcode_text_id_9221 - soft WebSocket TEXT opcode catalog id.
 *
 * Always returns 0x1. Soft continuum constant; does not parse WS
 * frames or open sockets. No parent wires.
 */
uint32_t
gj_ws_opcode_text_id_9221(void)
{
	(void)NULL;
	return b9221_opcode_text_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ws_opcode_text_id_9221(void)
    __attribute__((alias("gj_ws_opcode_text_id_9221")));
