/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7246: setsockopt SO_SNDBUF option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_sndbuf_id_7246(void);
 *     - Return soft SO_SNDBUF option constant (7).
 *   uint32_t __gj_so_sndbuf_id_7246  (alias)
 *   __libcgj_batch7246_marker = "libcgj-batch7246"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_sndbuf_id_7246 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7246_marker[] = "libcgj-batch7246";

/* SO_SNDBUF: send buffer size (value 7 on Linux). */
#define B7246_SO_SNDBUF ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7246_so_sndbuf_id(void)
{
	return B7246_SO_SNDBUF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_sndbuf_id_7246 - soft SO_SNDBUF option constant.
 *
 * Always returns 7 (Linux SO_SNDBUF). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_sndbuf_id_7246(void)
{
	(void)NULL;
	return b7246_so_sndbuf_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_sndbuf_id_7246(void)
    __attribute__((alias("gj_so_sndbuf_id_7246")));
