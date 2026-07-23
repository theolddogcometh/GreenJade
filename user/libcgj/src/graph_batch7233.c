/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7233: getsockopt SOL_IPV6 level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_ipv6_id_7233(void);
 *     - Return soft SOL_IPV6 / IPPROTO_IPV6 constant (41).
 *   uint32_t __gj_sol_ipv6_id_7233  (alias)
 *   __libcgj_batch7233_marker = "libcgj-batch7233"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_ipv6_id_7233 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7233_marker[] = "libcgj-batch7233";

/* SOL_IPV6 / IPPROTO_IPV6: IPv6 protocol-level options (value 41). */
#define B7233_SOL_IPV6 ((uint32_t)41u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7233_sol_ipv6_id(void)
{
	return B7233_SOL_IPV6;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_ipv6_id_7233 - soft SOL_IPV6 level constant.
 *
 * Always returns 41 (Linux SOL_IPV6 / IPPROTO_IPV6). Catalog id only;
 * does not call getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_ipv6_id_7233(void)
{
	(void)NULL;
	return b7233_sol_ipv6_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_ipv6_id_7233(void)
    __attribute__((alias("gj_sol_ipv6_id_7233")));
