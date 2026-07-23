/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8479: IP version-4 predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ip_ver4_u_8479(uint32_t ver);
 *     - Return 1 if ver is IPv4 (4); else 0.
 *   int __gj_ip_ver4_u_8479  (alias)
 *   __libcgj_batch8479_marker = "libcgj-batch8479"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_ip_ver4_u_8479 surface only; no multi-def. Soft IP version
 * check (IPv4 = 4); does not parse packets. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8479_marker[] = "libcgj-batch8479";

/* IPv4 version nibble / field value. */
#define B8479_IP_VER4 ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Ver is IPv4 (4). */
static int
b8479_ip_ver4(uint32_t u32Ver)
{
	if (u32Ver == B8479_IP_VER4) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_ver4_u_8479 - 1 iff ver is IPv4 (4).
 *
 * Edge cases:
 *   4 → 1
 *   0 / 6 / any other → 0
 */
int
gj_ip_ver4_u_8479(uint32_t u32Ver)
{
	(void)NULL;
	return b8479_ip_ver4(u32Ver);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip_ver4_u_8479(uint32_t u32Ver)
    __attribute__((alias("gj_ip_ver4_u_8479")));
