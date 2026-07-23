/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7242: setsockopt SO_REUSEADDR option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_reuseaddr_id_7242(void);
 *     - Return soft SO_REUSEADDR option constant (2).
 *   uint32_t __gj_so_reuseaddr_id_7242  (alias)
 *   __libcgj_batch7242_marker = "libcgj-batch7242"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_reuseaddr_id_7242 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7242_marker[] = "libcgj-batch7242";

/* SO_REUSEADDR: allow local address reuse (value 2 on Linux). */
#define B7242_SO_REUSEADDR ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7242_so_reuseaddr_id(void)
{
	return B7242_SO_REUSEADDR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_reuseaddr_id_7242 - soft SO_REUSEADDR option constant.
 *
 * Always returns 2 (Linux SO_REUSEADDR). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_reuseaddr_id_7242(void)
{
	(void)NULL;
	return b7242_so_reuseaddr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_reuseaddr_id_7242(void)
    __attribute__((alias("gj_so_reuseaddr_id_7242")));
