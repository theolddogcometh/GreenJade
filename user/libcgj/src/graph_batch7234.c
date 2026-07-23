/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7234: getsockopt SOL_TCP level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_tcp_id_7234(void);
 *     - Return soft SOL_TCP / IPPROTO_TCP constant (6).
 *   uint32_t __gj_sol_tcp_id_7234  (alias)
 *   __libcgj_batch7234_marker = "libcgj-batch7234"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_tcp_id_7234 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7234_marker[] = "libcgj-batch7234";

/* SOL_TCP / IPPROTO_TCP: TCP protocol-level options (value 6). */
#define B7234_SOL_TCP ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7234_sol_tcp_id(void)
{
	return B7234_SOL_TCP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_tcp_id_7234 - soft SOL_TCP level constant.
 *
 * Always returns 6 (Linux SOL_TCP / IPPROTO_TCP). Catalog id only; does
 * not call getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_tcp_id_7234(void)
{
	(void)NULL;
	return b7234_sol_tcp_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_tcp_id_7234(void)
    __attribute__((alias("gj_sol_tcp_id_7234")));
