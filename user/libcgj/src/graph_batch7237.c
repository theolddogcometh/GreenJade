/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7237: getsockopt SOL_PACKET level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_packet_id_7237(void);
 *     - Return soft SOL_PACKET constant (263).
 *   uint32_t __gj_sol_packet_id_7237  (alias)
 *   __libcgj_batch7237_marker = "libcgj-batch7237"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_packet_id_7237 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7237_marker[] = "libcgj-batch7237";

/* SOL_PACKET: AF_PACKET socket options (value 263 on Linux). */
#define B7237_SOL_PACKET ((uint32_t)263u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7237_sol_packet_id(void)
{
	return B7237_SOL_PACKET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_packet_id_7237 - soft SOL_PACKET level constant.
 *
 * Always returns 263 (Linux SOL_PACKET). Catalog id only; does not call
 * getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_packet_id_7237(void)
{
	(void)NULL;
	return b7237_sol_packet_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_packet_id_7237(void)
    __attribute__((alias("gj_sol_packet_id_7237")));
