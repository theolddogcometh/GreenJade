/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7248: setsockopt is-SOL_SOCKET level predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_is_sol_socket_u_7248(uint32_t level);
 *     - Return 1 if level is SOL_SOCKET (1), else 0.
 *   uint32_t __gj_sock_is_sol_socket_u_7248  (alias)
 *   __libcgj_batch7248_marker = "libcgj-batch7248"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_sock_is_sol_socket_u_7248 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7248_marker[] = "libcgj-batch7248";

/* SOL_SOCKET: socket-level options (value 1 on Linux). */
#define B7248_SOL_SOCKET ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7248_sock_is_sol_socket(uint32_t u32Level)
{
	return (u32Level == B7248_SOL_SOCKET) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_is_sol_socket_u_7248 - 1 if level is SOL_SOCKET.
 *
 * level: setsockopt(2) / getsockopt(2) level argument
 *
 * Returns 1 when level equals SOL_SOCKET (1), else 0.
 * Pure integer compare; does not call setsockopt. No parent wires.
 */
uint32_t
gj_sock_is_sol_socket_u_7248(uint32_t u32Level)
{
	(void)NULL;
	return b7248_sock_is_sol_socket(u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_is_sol_socket_u_7248(uint32_t u32Level)
    __attribute__((alias("gj_sock_is_sol_socket_u_7248")));
