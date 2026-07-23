/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8473: sockaddr AF_UNIX family id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_unix_id_8473(void);
 *     - Return Linux AF_UNIX / AF_LOCAL catalog value (1).
 *   uint32_t __gj_af_unix_id_8473  (alias)
 *   __libcgj_batch8473_marker = "libcgj-batch8473"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_af_unix_id_8473 surface only; no multi-def. Soft catalog
 * of AF_UNIX; does not open sockets or include sys/socket.h.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8473_marker[] = "libcgj-batch8473";

/* Linux AF_UNIX / AF_LOCAL soft catalog value. */
#define B8473_AF_UNIX ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8473_af_unix_id(void)
{
	return B8473_AF_UNIX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_unix_id_8473 - soft AF_UNIX family id catalog.
 *
 * Always returns 1 (Linux AF_UNIX / AF_LOCAL). Integer-only sockaddr
 * family stub; does not call socket(2). No parent wires.
 */
uint32_t
gj_af_unix_id_8473(void)
{
	(void)NULL;
	return b8473_af_unix_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_unix_id_8473(void)
    __attribute__((alias("gj_af_unix_id_8473")));
