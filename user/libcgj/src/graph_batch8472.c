/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8472: sockaddr AF_INET6 family id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_inet6_id_8472(void);
 *     - Return Linux AF_INET6 catalog value (10).
 *   uint32_t __gj_af_inet6_id_8472  (alias)
 *   __libcgj_batch8472_marker = "libcgj-batch8472"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_af_inet6_id_8472 surface only; no multi-def. Soft catalog
 * of AF_INET6; does not open sockets or include sys/socket.h.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8472_marker[] = "libcgj-batch8472";

/* Linux AF_INET6 soft catalog value. */
#define B8472_AF_INET6 ((uint32_t)10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8472_af_inet6_id(void)
{
	return B8472_AF_INET6;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_inet6_id_8472 - soft AF_INET6 family id catalog.
 *
 * Always returns 10 (Linux AF_INET6). Integer-only sockaddr family
 * stub; does not call socket(2). No parent wires.
 */
uint32_t
gj_af_inet6_id_8472(void)
{
	(void)NULL;
	return b8472_af_inet6_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_inet6_id_8472(void)
    __attribute__((alias("gj_af_inet6_id_8472")));
