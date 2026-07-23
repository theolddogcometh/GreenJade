/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8471: sockaddr AF_INET family id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_inet_id_8471(void);
 *     - Return Linux AF_INET catalog value (2).
 *   uint32_t __gj_af_inet_id_8471  (alias)
 *   __libcgj_batch8471_marker = "libcgj-batch8471"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_af_inet_id_8471 surface only; no multi-def. Soft catalog
 * of AF_INET; does not open sockets or include sys/socket.h.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8471_marker[] = "libcgj-batch8471";

/* Linux AF_INET soft catalog value. */
#define B8471_AF_INET ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8471_af_inet_id(void)
{
	return B8471_AF_INET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_inet_id_8471 - soft AF_INET family id catalog.
 *
 * Always returns 2 (Linux AF_INET). Integer-only sockaddr family stub;
 * does not call socket(2). No parent wires.
 */
uint32_t
gj_af_inet_id_8471(void)
{
	(void)NULL;
	return b8471_af_inet_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_inet_id_8471(void)
    __attribute__((alias("gj_af_inet_id_8471")));
