/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7250: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7250(void);
 *     - Returns the compile-time graph batch number for this TU (7250).
 *   uint32_t __gj_batch_id_7250  (alias)
 *   __libcgj_batch7250_marker = "libcgj-batch7250"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7250_marker[] = "libcgj-batch7250";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7250_id(void)
{
	return 7250u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7250 - report this TU's graph batch number.
 *
 * Always returns 7250. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7250(void)
{
	(void)NULL;
	return b7250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7250(void)
    __attribute__((alias("gj_batch_id_7250")));
