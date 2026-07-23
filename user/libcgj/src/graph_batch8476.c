/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8476: IPv4 INADDR_ANY ok predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ip4_any_ok_u_8476(uint32_t addr);
 *     - Return 1 if addr is INADDR_ANY (0.0.0.0 / 0); else 0.
 *   int __gj_ip4_any_ok_u_8476  (alias)
 *   __libcgj_batch8476_marker = "libcgj-batch8476"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_ip4_any_ok_u_8476 surface only; no multi-def. Treats
 * addr as a host-order IPv4 word. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8476_marker[] = "libcgj-batch8476";

/* Soft INADDR_ANY (0.0.0.0). */
#define B8476_INADDR_ANY ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Addr is the IPv4 any-address (0). */
static int
b8476_ip4_any_ok(uint32_t u32Addr)
{
	if (u32Addr == B8476_INADDR_ANY) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip4_any_ok_u_8476 - 1 iff addr is IPv4 INADDR_ANY.
 *
 * Edge cases:
 *   0 → 1
 *   any nonzero → 0
 */
int
gj_ip4_any_ok_u_8476(uint32_t u32Addr)
{
	(void)NULL;
	return b8476_ip4_any_ok(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip4_any_ok_u_8476(uint32_t u32Addr)
    __attribute__((alias("gj_ip4_any_ok_u_8476")));
