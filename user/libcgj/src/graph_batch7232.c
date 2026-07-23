/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7232: getsockopt SOL_IP level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_ip_id_7232(void);
 *     - Return soft SOL_IP / IPPROTO_IP constant (0).
 *   uint32_t __gj_sol_ip_id_7232  (alias)
 *   __libcgj_batch7232_marker = "libcgj-batch7232"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_ip_id_7232 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7232_marker[] = "libcgj-batch7232";

/* SOL_IP / IPPROTO_IP: IPv4 protocol-level options (value 0 on Linux). */
#define B7232_SOL_IP ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7232_sol_ip_id(void)
{
	return B7232_SOL_IP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_ip_id_7232 - soft SOL_IP level constant.
 *
 * Always returns 0 (Linux SOL_IP / IPPROTO_IP). Catalog id only; does not
 * call getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_ip_id_7232(void)
{
	(void)NULL;
	return b7232_sol_ip_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_ip_id_7232(void)
    __attribute__((alias("gj_sol_ip_id_7232")));
