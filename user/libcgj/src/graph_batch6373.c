/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6373: remote play session host pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rplay_session_host_pack_6373(uint32_t ipv4_host,
 *                                            uint32_t port16);
 *     - Soft host key: (ipv4_host & 0xffff0000) | (port16 & 0xffff).
 *       Pure fold for soft identity; not a wire format.
 *   uint32_t __gj_rplay_session_host_pack_6373  (alias)
 *   __libcgj_batch6373_marker = "libcgj-batch6373"
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

const char __libcgj_batch6373_marker[] = "libcgj-batch6373";

#define B6373_PORT_MASK  0x0000ffffu
#define B6373_HOST_MASK  0xffff0000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6373_host_pack(uint32_t u32Ip, uint32_t u32Port)
{
	return (u32Ip & B6373_HOST_MASK) | (u32Port & B6373_PORT_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rplay_session_host_pack_6373 - soft remote-play host key fold.
 *
 * ipv4_host: host-order IPv4 address bits
 * port16:    transport port (low 16 bits used)
 *
 * Returns packed soft key. Pure integer; no sockets. No parent wires.
 */
uint32_t
gj_rplay_session_host_pack_6373(uint32_t ipv4_host, uint32_t port16)
{
	(void)NULL;
	return b6373_host_pack(ipv4_host, port16);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rplay_session_host_pack_6373(uint32_t ipv4_host,
    uint32_t port16)
    __attribute__((alias("gj_rplay_session_host_pack_6373")));
