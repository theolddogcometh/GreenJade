/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9227: soft WebSocket mask-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ws_mask_ok_u_9227(void);
 *     - Returns 1 (ws mask soft-id continuum ok). Pure-data product
 *       tag; does not mask/unmask payloads or open sockets.
 *   uint32_t __gj_ws_mask_ok_u_9227  (alias)
 *   __libcgj_batch9227_marker = "libcgj-batch9227"
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

const char __libcgj_batch9227_marker[] = "libcgj-batch9227";

#define B9227_WS_MASK_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9227_mask_ok(void)
{
	return B9227_WS_MASK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_mask_ok_u_9227 - WebSocket mask soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not mask/unmask
 * payloads or open sockets. No parent wires.
 */
uint32_t
gj_ws_mask_ok_u_9227(void)
{
	(void)NULL;
	return b9227_mask_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ws_mask_ok_u_9227(void)
    __attribute__((alias("gj_ws_mask_ok_u_9227")));
