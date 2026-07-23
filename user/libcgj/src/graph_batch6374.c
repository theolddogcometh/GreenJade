/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6374: remote play session port soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_port_ok_6374(uint32_t port);
 *     - Soft port gate: ok when port in [1024, 65535] (non-privileged
 *       unicast). 0 and [1,1023] → 0; >65535 → 0; else 1.
 *   uint32_t __gj_rplay_session_port_ok_6374  (alias)
 *   __libcgj_batch6374_marker = "libcgj-batch6374"
 *
 * Exclusive continuum CREATE-ONLY (6371-6380: remote play session stubs —
 * session_id_ok_6371, token_len_ok_6372, host_pack_6373, port_ok_6374,
 * latency_budget_6375, bitrate_score_6376, peer_count_ok_6377,
 * state_pack_6378, heartbeat_ms_6379, session_ready_6380). Unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6374_marker[] = "libcgj-batch6374";

#define B6374_PORT_MIN  1024u
#define B6374_PORT_MAX  65535u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6374_port_ok(uint32_t u32Port)
{
	if (u32Port < B6374_PORT_MIN || u32Port > B6374_PORT_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_port_ok_6374 - soft remote-play port validity.
 *
 * port: candidate transport port number
 *
 * Returns 1 when port is in [1024, 65535], else 0. No sockets.
 * No parent wires.
 */
uint32_t
gj_rplay_session_port_ok_6374(uint32_t port)
{
	(void)NULL;
	return b6374_port_ok(port);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_port_ok_6374(uint32_t port)
    __attribute__((alias("gj_rplay_session_port_ok_6374")));
