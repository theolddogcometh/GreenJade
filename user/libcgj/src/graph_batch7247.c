/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7247: setsockopt SO_LINGER option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_linger_id_7247(void);
 *     - Return soft SO_LINGER option constant (13).
 *   uint32_t __gj_so_linger_id_7247  (alias)
 *   __libcgj_batch7247_marker = "libcgj-batch7247"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_linger_id_7247 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7247_marker[] = "libcgj-batch7247";

/* SO_LINGER: linger on close if data present (value 13 on Linux). */
#define B7247_SO_LINGER ((uint32_t)13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7247_so_linger_id(void)
{
	return B7247_SO_LINGER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_linger_id_7247 - soft SO_LINGER option constant.
 *
 * Always returns 13 (Linux SO_LINGER). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_linger_id_7247(void)
{
	(void)NULL;
	return b7247_so_linger_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_linger_id_7247(void)
    __attribute__((alias("gj_so_linger_id_7247")));
