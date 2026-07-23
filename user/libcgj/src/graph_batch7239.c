/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7239: getsockopt level soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sockopt_level_ok_u_7239(uint32_t level);
 *     - Return 1 if level is a known soft SOL_* catalog entry, else 0.
 *   uint32_t __gj_sockopt_level_ok_u_7239  (alias)
 *   __libcgj_batch7239_marker = "libcgj-batch7239"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sockopt_level_ok_u_7239 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7239_marker[] = "libcgj-batch7239";

/*
 * Soft catalog levels (Linux getsockopt/setsockopt level):
 *   SOL_IP=0, SOL_SOCKET=1, SOL_TCP=6, SOL_UDP=17,
 *   SOL_IPV6=41, SOL_RAW=255, SOL_PACKET=263
 */
#define B7239_SOL_IP     ((uint32_t)0u)
#define B7239_SOL_SOCKET ((uint32_t)1u)
#define B7239_SOL_TCP    ((uint32_t)6u)
#define B7239_SOL_UDP    ((uint32_t)17u)
#define B7239_SOL_IPV6   ((uint32_t)41u)
#define B7239_SOL_RAW    ((uint32_t)255u)
#define B7239_SOL_PACKET ((uint32_t)263u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7239_sockopt_level_ok(uint32_t u32Level)
{
	return (u32Level == B7239_SOL_IP ||
	        u32Level == B7239_SOL_SOCKET ||
	        u32Level == B7239_SOL_TCP ||
	        u32Level == B7239_SOL_UDP ||
	        u32Level == B7239_SOL_IPV6 ||
	        u32Level == B7239_SOL_RAW ||
	        u32Level == B7239_SOL_PACKET) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sockopt_level_ok_u_7239 - 1 if level is soft-known SOL_*.
 *
 * level: getsockopt(2)/setsockopt(2) level argument
 *
 * Returns 1 when level is in {SOL_IP, SOL_SOCKET, SOL_TCP, SOL_UDP,
 * SOL_IPV6, SOL_RAW, SOL_PACKET}, else 0.
 * Soft catalog check; does not call getsockopt. No parent wires.
 */
uint32_t
gj_sockopt_level_ok_u_7239(uint32_t u32Level)
{
	(void)NULL;
	return b7239_sockopt_level_ok(u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sockopt_level_ok_u_7239(uint32_t u32Level)
    __attribute__((alias("gj_sockopt_level_ok_u_7239")));
