/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8477: IPv4 loopback ok predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ip4_loop_ok_u_8477(uint32_t addr);
 *     - Return 1 if addr is in 127.0.0.0/8 (host-order high byte 0x7f);
 *       else 0.
 *   int __gj_ip4_loop_ok_u_8477  (alias)
 *   __libcgj_batch8477_marker = "libcgj-batch8477"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_ip4_loop_ok_u_8477 surface only; no multi-def. Treats
 * addr as a host-order IPv4 word (0x7f000001 = 127.0.0.1).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8477_marker[] = "libcgj-batch8477";

/* Host-order 127.0.0.0/8 prefix mask and match. */
#define B8477_LOOP_MASK  ((uint32_t)0xff000000u)
#define B8477_LOOP_PREF  ((uint32_t)0x7f000000u)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Addr is in 127.0.0.0/8 (host-order). */
static int
b8477_ip4_loop_ok(uint32_t u32Addr)
{
	if ((u32Addr & B8477_LOOP_MASK) == B8477_LOOP_PREF) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip4_loop_ok_u_8477 - 1 iff addr is IPv4 loopback (127/8).
 *
 * Edge cases:
 *   0x7f000001 (127.0.0.1) → 1
 *   0x7f000000..0x7fffffff → 1
 *   0 / 0x0a000001 / others → 0
 */
int
gj_ip4_loop_ok_u_8477(uint32_t u32Addr)
{
	(void)NULL;
	return b8477_ip4_loop_ok(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip4_loop_ok_u_8477(uint32_t u32Addr)
    __attribute__((alias("gj_ip4_loop_ok_u_8477")));
