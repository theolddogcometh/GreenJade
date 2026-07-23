/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4939: IPv4 loopback predicate (127.0.0.0/8).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ip4_is_loopback_u(uint32_t ip);
 *     - Return 1 when host-order ip is in 127.0.0.0/8
 *       ((ip >> 24) == 0x7f); else 0.
 *   uint32_t __gj_ip4_is_loopback_u  (alias)
 *   __libcgj_batch4939_marker = "libcgj-batch4939"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940). Distinct
 * from gj_ipv4_is_loopback (batch859, int return) — unique
 * gj_ip4_is_loopback_u uint32 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4939_marker[] = "libcgj-batch4939";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4939_is_loopback(uint32_t u32Ip)
{
	if (((u32Ip >> 24) & 0xffu) == 0x7fu) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip4_is_loopback_u - true (1) when host-order IPv4 is in 127.0.0.0/8.
 *
 * ip: host-order packed address (0xAABBCCDD = a.b.c.d)
 *
 * High octet == 127 ⇔ loopback block (includes 127.0.0.1 and full /8).
 * Returns 1 for loopback, 0 otherwise. Self-contained; no parent wires.
 */
uint32_t
gj_ip4_is_loopback_u(uint32_t u32Ip)
{
	(void)NULL;
	return b4939_is_loopback(u32Ip);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ip4_is_loopback_u(uint32_t u32Ip)
    __attribute__((alias("gj_ip4_is_loopback_u")));
