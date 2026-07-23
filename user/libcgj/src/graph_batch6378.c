/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6378: remote play session state pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_state_pack_6378(uint32_t phase,
 *                                             uint32_t flags);
 *     - Soft state word: (phase & 0xff) | ((flags & 0xffffff) << 8).
 *       phase soft codes 0..255; flags low 24 bits only.
 *   uint32_t __gj_rplay_session_state_pack_6378  (alias)
 *   __libcgj_batch6378_marker = "libcgj-batch6378"
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

const char __libcgj_batch6378_marker[] = "libcgj-batch6378";

#define B6378_PHASE_MASK  0x000000ffu
#define B6378_FLAGS_MASK  0x00ffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6378_state_pack(uint32_t u32Phase, uint32_t u32Flags)
{
	return (u32Phase & B6378_PHASE_MASK) |
	    ((u32Flags & B6378_FLAGS_MASK) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_state_pack_6378 - soft remote-play state word.
 *
 * phase: session phase code (low 8 bits retained)
 * flags: session flags (low 24 bits retained, shifted to bits 8..31)
 *
 * Returns packed soft state. Pure integer. No parent wires.
 */
uint32_t
gj_rplay_session_state_pack_6378(uint32_t phase, uint32_t flags)
{
	(void)NULL;
	return b6378_state_pack(phase, flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_state_pack_6378(uint32_t phase, uint32_t flags)
    __attribute__((alias("gj_rplay_session_state_pack_6378")));
