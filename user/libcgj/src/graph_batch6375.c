/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6375: remote play session latency budget stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_latency_budget_6375(uint32_t rtt_ms,
 *                                                 uint32_t jitter_ms);
 *     - Soft one-way budget ms: rtt_ms/2 + jitter_ms, saturating at
 *       10000 ms. Pure soft estimate for session QoS stubs.
 *   uint32_t __gj_rplay_session_latency_budget_6375  (alias)
 *   __libcgj_batch6375_marker = "libcgj-batch6375"
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

const char __libcgj_batch6375_marker[] = "libcgj-batch6375";

#define B6375_BUDGET_CAP  10000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6375_budget(uint32_t u32Rtt, uint32_t u32Jitter)
{
	uint32_t u32Half;
	uint32_t u32Sum;

	u32Half = u32Rtt / 2u;
	if (u32Jitter > (B6375_BUDGET_CAP - u32Half)) {
		return B6375_BUDGET_CAP;
	}
	u32Sum = u32Half + u32Jitter;
	if (u32Sum > B6375_BUDGET_CAP) {
		return B6375_BUDGET_CAP;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_latency_budget_6375 - soft one-way latency budget ms.
 *
 * rtt_ms:    measured round-trip time in milliseconds
 * jitter_ms: soft jitter allowance in milliseconds
 *
 * Returns rtt/2 + jitter, capped at 10000. Pure integer. No parent wires.
 */
uint32_t
gj_rplay_session_latency_budget_6375(uint32_t rtt_ms, uint32_t jitter_ms)
{
	(void)NULL;
	return b6375_budget(rtt_ms, jitter_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_latency_budget_6375(uint32_t rtt_ms,
    uint32_t jitter_ms)
    __attribute__((alias("gj_rplay_session_latency_budget_6375")));
