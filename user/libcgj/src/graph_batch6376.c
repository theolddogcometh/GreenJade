/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6376: remote play session bitrate score stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_bitrate_score_6376(uint32_t kbps,
 *                                                uint32_t target_kbps);
 *     - Soft product score in [0, 100]: when target_kbps == 0 → 0;
 *       else min(100, (kbps * 100) / target_kbps). Caps kbps contrib
 *       when kbps >= target.
 *   uint32_t __gj_rplay_session_bitrate_score_6376  (alias)
 *   __libcgj_batch6376_marker = "libcgj-batch6376"
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

const char __libcgj_batch6376_marker[] = "libcgj-batch6376";

#define B6376_SCORE_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6376_score(uint32_t u32Kbps, uint32_t u32Target)
{
	uint32_t u32Score;

	if (u32Target == 0u) {
		return 0u;
	}
	if (u32Kbps >= u32Target) {
		return B6376_SCORE_MAX;
	}
	/* kbps < target → product fits in 32 bits for practical rates */
	u32Score = (u32Kbps * B6376_SCORE_MAX) / u32Target;
	if (u32Score > B6376_SCORE_MAX) {
		return B6376_SCORE_MAX;
	}
	return u32Score;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_bitrate_score_6376 - soft bitrate adequacy score.
 *
 * kbps:        observed encode/stream kilobits per second
 * target_kbps: session target kilobits per second
 *
 * Returns soft score in [0, 100]. Pure integer. No parent wires.
 */
uint32_t
gj_rplay_session_bitrate_score_6376(uint32_t kbps, uint32_t target_kbps)
{
	(void)NULL;
	return b6376_score(kbps, target_kbps);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_bitrate_score_6376(uint32_t kbps,
    uint32_t target_kbps)
    __attribute__((alias("gj_rplay_session_bitrate_score_6376")));
