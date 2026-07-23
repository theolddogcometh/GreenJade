/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7240: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7240(void);
 *     - Returns the compile-time graph batch number for this TU (7240).
 *   uint32_t __gj_batch_id_7240  (alias)
 *   __libcgj_batch7240_marker = "libcgj-batch7240"
 *
 * Exclusive continuum CREATE-ONLY (7231-7240: getsockopt level stubs —
 * sol_socket_id, sol_ip_id, sol_ipv6_id, sol_tcp_id, sol_udp_id,
 * sol_raw_id, sol_packet_id, is_sol_socket, level_ok, batch_id_7240).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7240_marker[] = "libcgj-batch7240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7240_id(void)
{
	return 7240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7240 - report this TU's graph batch number.
 *
 * Always returns 7240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7240(void)
{
	(void)NULL;
	return b7240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7240(void)
    __attribute__((alias("gj_batch_id_7240")));
