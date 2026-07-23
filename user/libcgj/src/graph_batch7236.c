/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7236: getsockopt SOL_RAW level id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sol_raw_id_7236(void);
 *     - Return soft SOL_RAW constant (255).
 *   uint32_t __gj_sol_raw_id_7236  (alias)
 *   __libcgj_batch7236_marker = "libcgj-batch7236"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Unique gj_sol_raw_id_7236 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7236_marker[] = "libcgj-batch7236";

/* SOL_RAW: raw socket protocol-level options (value 255 on Linux). */
#define B7236_SOL_RAW ((uint32_t)255u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7236_sol_raw_id(void)
{
	return B7236_SOL_RAW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sol_raw_id_7236 - soft SOL_RAW level constant.
 *
 * Always returns 255 (Linux SOL_RAW). Catalog id only; does not call
 * getsockopt/setsockopt. No parent wires.
 */
uint32_t
gj_sol_raw_id_7236(void)
{
	(void)NULL;
	return b7236_sol_raw_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sol_raw_id_7236(void)
    __attribute__((alias("gj_sol_raw_id_7236")));
