/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7243: setsockopt SO_KEEPALIVE option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_keepalive_id_7243(void);
 *     - Return soft SO_KEEPALIVE option constant (9).
 *   uint32_t __gj_so_keepalive_id_7243  (alias)
 *   __libcgj_batch7243_marker = "libcgj-batch7243"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_keepalive_id_7243 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7243_marker[] = "libcgj-batch7243";

/* SO_KEEPALIVE: keep connections alive (value 9 on Linux). */
#define B7243_SO_KEEPALIVE ((uint32_t)9u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7243_so_keepalive_id(void)
{
	return B7243_SO_KEEPALIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_keepalive_id_7243 - soft SO_KEEPALIVE option constant.
 *
 * Always returns 9 (Linux SO_KEEPALIVE). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_keepalive_id_7243(void)
{
	(void)NULL;
	return b7243_so_keepalive_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_keepalive_id_7243(void)
    __attribute__((alias("gj_so_keepalive_id_7243")));
