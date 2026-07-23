/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8475: socket SOCK_DGRAM type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_dgram_id_8475(void);
 *     - Return Linux SOCK_DGRAM catalog value (2).
 *   uint32_t __gj_sock_dgram_id_8475  (alias)
 *   __libcgj_batch8475_marker = "libcgj-batch8475"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_sock_dgram_id_8475 surface only; no multi-def. Soft
 * catalog of SOCK_DGRAM; does not open sockets or include
 * sys/socket.h. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8475_marker[] = "libcgj-batch8475";

/* Linux SOCK_DGRAM soft catalog value. */
#define B8475_SOCK_DGRAM ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8475_sock_dgram_id(void)
{
	return B8475_SOCK_DGRAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_dgram_id_8475 - soft SOCK_DGRAM type id catalog.
 *
 * Always returns 2 (Linux SOCK_DGRAM). Integer-only socket type stub;
 * does not call socket(2). No parent wires.
 */
uint32_t
gj_sock_dgram_id_8475(void)
{
	(void)NULL;
	return b8475_sock_dgram_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_dgram_id_8475(void)
    __attribute__((alias("gj_sock_dgram_id_8475")));
