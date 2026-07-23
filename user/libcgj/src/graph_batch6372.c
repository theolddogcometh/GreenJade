/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6372: remote play session token length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_token_len_ok_6372(uint32_t token_len);
 *     - Soft token length gate: ok when len in [16, 256] inclusive.
 *       Outside range → 0, else 1. No crypto, pure bounds.
 *   uint32_t __gj_rplay_session_token_len_ok_6372  (alias)
 *   __libcgj_batch6372_marker = "libcgj-batch6372"
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

const char __libcgj_batch6372_marker[] = "libcgj-batch6372";

#define B6372_TOKEN_MIN  16u
#define B6372_TOKEN_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6372_token_len_ok(uint32_t u32Len)
{
	if (u32Len < B6372_TOKEN_MIN || u32Len > B6372_TOKEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_token_len_ok_6372 - soft session token length gate.
 *
 * token_len: bearer/session token byte length
 *
 * Returns 1 when len is in [16, 256], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_rplay_session_token_len_ok_6372(uint32_t token_len)
{
	(void)NULL;
	return b6372_token_len_ok(token_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_token_len_ok_6372(uint32_t token_len)
    __attribute__((alias("gj_rplay_session_token_len_ok_6372")));
