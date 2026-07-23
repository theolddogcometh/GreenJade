/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6380: remote play session ready soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_ready_6380(uint32_t id_ok,
 *                                        uint32_t port_ok,
 *                                        uint32_t peer_ok,
 *                                        uint32_t bitrate_score);
 *     - Soft session-ready gate: all of id_ok, port_ok, peer_ok must
 *       be non-zero and bitrate_score >= 50 → 1; else 0.
 *   uint32_t __gj_rplay_session_ready_6380  (alias)
 *   uint32_t gj_batch_id_6380(void);
 *   uint32_t __gj_batch_id_6380  (alias)
 *   __libcgj_batch6380_marker = "libcgj-batch6380"
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

const char __libcgj_batch6380_marker[] = "libcgj-batch6380";

#define B6380_BITRATE_MIN  50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6380_ready(uint32_t u32IdOk, uint32_t u32PortOk, uint32_t u32PeerOk,
    uint32_t u32Bitrate)
{
	if (u32IdOk == 0u || u32PortOk == 0u || u32PeerOk == 0u) {
		return 0u;
	}
	if (u32Bitrate < B6380_BITRATE_MIN) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_ready_6380 - soft remote-play session ready gate.
 *
 * id_ok:         non-zero when session id soft-ok
 * port_ok:       non-zero when port soft-ok
 * peer_ok:       non-zero when peer count soft-ok
 * bitrate_score: soft score [0, 100]; need >= 50
 *
 * Returns 1 when all gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_rplay_session_ready_6380(uint32_t id_ok, uint32_t port_ok,
    uint32_t peer_ok, uint32_t bitrate_score)
{
	(void)NULL;
	return b6380_ready(id_ok, port_ok, peer_ok, bitrate_score);
}

/*
 * gj_batch_id_6380 - report this TU's graph batch number.
 *
 * Always returns 6380.
 */
uint32_t
gj_batch_id_6380(void)
{
	return 6380u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rplay_session_ready_6380(uint32_t id_ok, uint32_t port_ok,
    uint32_t peer_ok, uint32_t bitrate_score)
    __attribute__((alias("gj_rplay_session_ready_6380")));

uint32_t __gj_batch_id_6380(void)
    __attribute__((alias("gj_batch_id_6380")));
