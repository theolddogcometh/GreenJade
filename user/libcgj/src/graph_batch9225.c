/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9225: soft WebSocket pong opcode catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ws_opcode_pong_id_9225(void);
 *     - Returns 0xA (RFC 6455 opcode PONG soft catalog value). Pure-data
 *       opcode id stub; does not parse frames or open sockets.
 *   uint32_t __gj_ws_opcode_pong_id_9225  (alias)
 *   __libcgj_batch9225_marker = "libcgj-batch9225"
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

const char __libcgj_batch9225_marker[] = "libcgj-batch9225";

/* Soft RFC 6455 opcode PONG (0xA). */
#define B9225_WS_OPCODE_PONG  0xAu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9225_opcode_pong_id(void)
{
	return B9225_WS_OPCODE_PONG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_opcode_pong_id_9225 - soft WebSocket PONG opcode catalog id.
 *
 * Always returns 0xA. Soft continuum constant; does not parse WS
 * frames or open sockets. No parent wires.
 */
uint32_t
gj_ws_opcode_pong_id_9225(void)
{
	(void)NULL;
	return b9225_opcode_pong_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ws_opcode_pong_id_9225(void)
    __attribute__((alias("gj_ws_opcode_pong_id_9225")));
