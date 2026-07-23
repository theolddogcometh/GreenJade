/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6379: remote play session heartbeat ms stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_heartbeat_ms_6379(uint32_t rtt_ms,
 *                                               uint32_t base_ms);
 *     - Soft heartbeat interval: base_ms + rtt_ms, clamped to
 *       [250, 30000] ms. base_ms of 0 uses default 1000.
 *   uint32_t __gj_rplay_session_heartbeat_ms_6379  (alias)
 *   __libcgj_batch6379_marker = "libcgj-batch6379"
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

const char __libcgj_batch6379_marker[] = "libcgj-batch6379";

#define B6379_BASE_DEFAULT  1000u
#define B6379_HB_MIN         250u
#define B6379_HB_MAX       30000u
#define B6379_U32_MAX  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6379_heartbeat(uint32_t u32Rtt, uint32_t u32Base)
{
	uint32_t u32B;
	uint32_t u32Sum;

	u32B = u32Base;
	if (u32B == 0u) {
		u32B = B6379_BASE_DEFAULT;
	}
	if (u32Rtt > (B6379_U32_MAX - u32B)) {
		u32Sum = B6379_HB_MAX;
	} else {
		u32Sum = u32B + u32Rtt;
	}
	if (u32Sum < B6379_HB_MIN) {
		return B6379_HB_MIN;
	}
	if (u32Sum > B6379_HB_MAX) {
		return B6379_HB_MAX;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_heartbeat_ms_6379 - soft heartbeat interval ms.
 *
 * rtt_ms:  recent RTT sample in milliseconds
 * base_ms: base interval (0 → default 1000)
 *
 * Returns base + rtt clamped to [250, 30000]. Pure integer.
 * No parent wires.
 */
uint32_t
gj_rplay_session_heartbeat_ms_6379(uint32_t rtt_ms, uint32_t base_ms)
{
	(void)NULL;
	return b6379_heartbeat(rtt_ms, base_ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_heartbeat_ms_6379(uint32_t rtt_ms,
    uint32_t base_ms)
    __attribute__((alias("gj_rplay_session_heartbeat_ms_6379")));
