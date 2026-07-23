/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6377: remote play session peer count stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_peer_count_ok_6377(uint32_t peers,
 *                                                uint32_t max_peers);
 *     - Soft peer-count gate: ok when max_peers in [1, 64] and
 *       peers in [1, max_peers]. Else 0.
 *   uint32_t __gj_rplay_session_peer_count_ok_6377  (alias)
 *   __libcgj_batch6377_marker = "libcgj-batch6377"
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

const char __libcgj_batch6377_marker[] = "libcgj-batch6377";

#define B6377_MAX_PEERS_CAP  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6377_peer_ok(uint32_t u32Peers, uint32_t u32Max)
{
	if (u32Max < 1u || u32Max > B6377_MAX_PEERS_CAP) {
		return 0u;
	}
	if (u32Peers < 1u || u32Peers > u32Max) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_peer_count_ok_6377 - soft peer-count validity.
 *
 * peers:     current remote-play peer count
 * max_peers: session capacity (must be in [1, 64])
 *
 * Returns 1 when peers is in [1, max_peers] and max is valid, else 0.
 * No parent wires.
 */
uint32_t
gj_rplay_session_peer_count_ok_6377(uint32_t peers, uint32_t max_peers)
{
	(void)NULL;
	return b6377_peer_ok(peers, max_peers);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_peer_count_ok_6377(uint32_t peers,
    uint32_t max_peers)
    __attribute__((alias("gj_rplay_session_peer_count_ok_6377")));
