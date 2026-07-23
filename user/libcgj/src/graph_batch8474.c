/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8474: socket SOCK_STREAM type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_stream_id_8474(void);
 *     - Return Linux SOCK_STREAM catalog value (1).
 *   uint32_t __gj_sock_stream_id_8474  (alias)
 *   __libcgj_batch8474_marker = "libcgj-batch8474"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_sock_stream_id_8474 surface only; no multi-def. Soft
 * catalog of SOCK_STREAM; does not open sockets or include
 * sys/socket.h. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8474_marker[] = "libcgj-batch8474";

/* Linux SOCK_STREAM soft catalog value. */
#define B8474_SOCK_STREAM ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8474_sock_stream_id(void)
{
	return B8474_SOCK_STREAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_stream_id_8474 - soft SOCK_STREAM type id catalog.
 *
 * Always returns 1 (Linux SOCK_STREAM). Integer-only socket type stub;
 * does not call socket(2). No parent wires.
 */
uint32_t
gj_sock_stream_id_8474(void)
{
	(void)NULL;
	return b8474_sock_stream_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_stream_id_8474(void)
    __attribute__((alias("gj_sock_stream_id_8474")));
