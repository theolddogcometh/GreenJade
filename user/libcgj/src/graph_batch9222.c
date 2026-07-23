/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9222: soft WebSocket binary opcode catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ws_opcode_bin_id_9222(void);
 *     - Returns 0x2 (RFC 6455 opcode BINARY soft catalog value). Pure-data
 *       opcode id stub; does not parse frames or open sockets.
 *   uint32_t __gj_ws_opcode_bin_id_9222  (alias)
 *   __libcgj_batch9222_marker = "libcgj-batch9222"
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

const char __libcgj_batch9222_marker[] = "libcgj-batch9222";

/* Soft RFC 6455 opcode BINARY (0x2). */
#define B9222_WS_OPCODE_BIN  0x2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9222_opcode_bin_id(void)
{
	return B9222_WS_OPCODE_BIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_opcode_bin_id_9222 - soft WebSocket BINARY opcode catalog id.
 *
 * Always returns 0x2. Soft continuum constant; does not parse WS
 * frames or open sockets. No parent wires.
 */
uint32_t
gj_ws_opcode_bin_id_9222(void)
{
	(void)NULL;
	return b9222_opcode_bin_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ws_opcode_bin_id_9222(void)
    __attribute__((alias("gj_ws_opcode_bin_id_9222")));
