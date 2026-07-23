/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4934: pack IPv4 octets into host-order word.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ip4_pack_u(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
 *     - Pack unsigned octet words (masked to 8 bits) into a single
 *       host-order IPv4 word 0xAABBCCDD = a.b.c.d.
 *   uint32_t __gj_ip4_pack_u  (alias)
 *   __libcgj_batch4934_marker = "libcgj-batch4934"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940:
 * sockaddr_family_inet_u, sockaddr_family_inet6_u,
 * sockaddr_family_unix_u, ip4_pack_u, ip4_a_u, ip4_b_u, ip4_c_u,
 * ip4_d_u, ip4_is_loopback_u, batch_id_4940). Distinct from
 * gj_ipv4_network (batch856) / gj_ipv4_broadcast (batch857) —
 * unique gj_ip4_pack_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4934_marker[] = "libcgj-batch4934";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4934_oct8(uint32_t u32V)
{
	return u32V & 0xffu;
}

static uint32_t
b4934_pack(uint32_t u32A, uint32_t u32B, uint32_t u32C, uint32_t u32D)
{
	return (b4934_oct8(u32A) << 24) |
	    (b4934_oct8(u32B) << 16) |
	    (b4934_oct8(u32C) << 8) |
	    b4934_oct8(u32D);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip4_pack_u - pack a.b.c.d into a host-order IPv4 word.
 *
 * a, b, c, d: octet values (only the low 8 bits are used)
 *
 * Layout (MSB→LSB): A | B | C | D. Host-order form matching
 * gj_ip4_a_u / gj_ip4_is_loopback_u extractors in this wave.
 * Self-contained; no parent wires.
 */
uint32_t
gj_ip4_pack_u(uint32_t u32A, uint32_t u32B, uint32_t u32C, uint32_t u32D)
{
	(void)NULL;
	return b4934_pack(u32A, u32B, u32C, u32D);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ip4_pack_u(uint32_t u32A, uint32_t u32B, uint32_t u32C,
    uint32_t u32D)
    __attribute__((alias("gj_ip4_pack_u")));
