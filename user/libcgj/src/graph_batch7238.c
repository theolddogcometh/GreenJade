/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7238: getsockopt is-SOL_SOCKET level predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sockopt_is_sol_socket_u_7238(uint32_t level);
 *     - Return 1 if level is SOL_SOCKET (1), else 0.
 *   uint32_t __gj_sockopt_is_sol_socket_u_7238  (alias)
 *   __libcgj_batch7238_marker = "libcgj-batch7238"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sockopt_is_sol_socket_u_7238 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7238_marker[] = "libcgj-batch7238";

/* SOL_SOCKET: socket-level options (value 1 on Linux). */
#define B7238_SOL_SOCKET ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7238_sockopt_is_sol_socket(uint32_t u32Level)
{
	return (u32Level == B7238_SOL_SOCKET) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sockopt_is_sol_socket_u_7238 - 1 if level is SOL_SOCKET.
 *
 * level: getsockopt(2)/setsockopt(2) level argument
 *
 * Returns 1 when level equals SOL_SOCKET (1), else 0.
 * Pure integer compare; does not call getsockopt. No parent wires.
 */
uint32_t
gj_sockopt_is_sol_socket_u_7238(uint32_t u32Level)
{
	(void)NULL;
	return b7238_sockopt_is_sol_socket(u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sockopt_is_sol_socket_u_7238(uint32_t u32Level)
    __attribute__((alias("gj_sockopt_is_sol_socket_u_7238")));
