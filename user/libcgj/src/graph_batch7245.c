/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7245: setsockopt SO_RCVBUF option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_rcvbuf_id_7245(void);
 *     - Return soft SO_RCVBUF option constant (8).
 *   uint32_t __gj_so_rcvbuf_id_7245  (alias)
 *   __libcgj_batch7245_marker = "libcgj-batch7245"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_rcvbuf_id_7245 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7245_marker[] = "libcgj-batch7245";

/* SO_RCVBUF: receive buffer size (value 8 on Linux). */
#define B7245_SO_RCVBUF ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7245_so_rcvbuf_id(void)
{
	return B7245_SO_RCVBUF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_rcvbuf_id_7245 - soft SO_RCVBUF option constant.
 *
 * Always returns 8 (Linux SO_RCVBUF). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_rcvbuf_id_7245(void)
{
	(void)NULL;
	return b7245_so_rcvbuf_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_rcvbuf_id_7245(void)
    __attribute__((alias("gj_so_rcvbuf_id_7245")));
