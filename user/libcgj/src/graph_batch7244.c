/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7244: setsockopt SO_BROADCAST option id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_so_broadcast_id_7244(void);
 *     - Return soft SO_BROADCAST option constant (6).
 *   uint32_t __gj_so_broadcast_id_7244  (alias)
 *   __libcgj_batch7244_marker = "libcgj-batch7244"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_so_broadcast_id_7244 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7244_marker[] = "libcgj-batch7244";

/* SO_BROADCAST: permit sending of broadcast messages (value 6 on Linux). */
#define B7244_SO_BROADCAST ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7244_so_broadcast_id(void)
{
	return B7244_SO_BROADCAST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_so_broadcast_id_7244 - soft SO_BROADCAST option constant.
 *
 * Always returns 6 (Linux SO_BROADCAST). Catalog id only; does not call
 * setsockopt. No parent wires.
 */
uint32_t
gj_so_broadcast_id_7244(void)
{
	(void)NULL;
	return b7244_so_broadcast_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_so_broadcast_id_7244(void)
    __attribute__((alias("gj_so_broadcast_id_7244")));
