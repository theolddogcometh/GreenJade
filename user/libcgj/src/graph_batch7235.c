/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7235: getsockopt SOL_UDP level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_udp_id_7235(void);
 *     - Return soft SOL_UDP / IPPROTO_UDP constant (17).
 *   uint32_t __gj_sol_udp_id_7235  (alias)
 *   __libcgj_batch7235_marker = "libcgj-batch7235"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_udp_id_7235 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7235_marker[] = "libcgj-batch7235";

/* SOL_UDP / IPPROTO_UDP: UDP protocol-level options (value 17). */
#define B7235_SOL_UDP ((uint32_t)17u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7235_sol_udp_id(void)
{
	return B7235_SOL_UDP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_udp_id_7235 - soft SOL_UDP level constant.
 *
 * Always returns 17 (Linux SOL_UDP / IPPROTO_UDP). Catalog id only; does
 * not call getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_udp_id_7235(void)
{
	(void)NULL;
	return b7235_sol_udp_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_udp_id_7235(void)
    __attribute__((alias("gj_sol_udp_id_7235")));
