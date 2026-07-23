/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9223: soft WebSocket close opcode catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ws_opcode_close_id_9223(void);
 *     - Returns 0x8 (RFC 6455 opcode CLOSE soft catalog value). Pure-data
 *       opcode id stub; does not parse frames or open sockets.
 *   uint32_t __gj_ws_opcode_close_id_9223  (alias)
 *   __libcgj_batch9223_marker = "libcgj-batch9223"
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

const char __libcgj_batch9223_marker[] = "libcgj-batch9223";

/* Soft RFC 6455 opcode CLOSE (0x8). */
#define B9223_WS_OPCODE_CLOSE  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9223_opcode_close_id(void)
{
	return B9223_WS_OPCODE_CLOSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_opcode_close_id_9223 - soft WebSocket CLOSE opcode catalog id.
 *
 * Always returns 0x8. Soft continuum constant; does not parse WS
 * frames or open sockets. No parent wires.
 */
uint32_t
gj_ws_opcode_close_id_9223(void)
{
	(void)NULL;
	return b9223_opcode_close_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ws_opcode_close_id_9223(void)
    __attribute__((alias("gj_ws_opcode_close_id_9223")));
