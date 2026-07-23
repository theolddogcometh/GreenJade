/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6371: remote play session id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_id_ok_6371(uint32_t session_id);
 *     - Soft non-zero session-id check: 0 → 0; else 1 when id is in
 *       [1, 0xfffffffe]. Reserved all-bits-one (0xffffffff) → 0.
 *   uint32_t __gj_rplay_session_id_ok_6371  (alias)
 *   __libcgj_batch6371_marker = "libcgj-batch6371"
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

const char __libcgj_batch6371_marker[] = "libcgj-batch6371";

#define B6371_ID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6371_id_ok(uint32_t u32Id)
{
	if (u32Id == 0u || u32Id > B6371_ID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_id_ok_6371 - soft remote-play session id validity.
 *
 * session_id: opaque 32-bit session handle
 *
 * Returns 1 when id is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_rplay_session_id_ok_6371(uint32_t session_id)
{
	(void)NULL;
	return b6371_id_ok(session_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_id_ok_6371(uint32_t session_id)
    __attribute__((alias("gj_rplay_session_id_ok_6371")));
