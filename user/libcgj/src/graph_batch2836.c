/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2836: host-order IPv4 multicast predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_is_ipv4_multicast_u(uint32_t addr);
 *     - 1 if host-order addr is in 224.0.0.0/4 (class D), else 0.
 *   int __gj_is_ipv4_multicast_u  (alias)
 *   __libcgj_batch2836_marker = "libcgj-batch2836"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from
 * gj_ipv4_is_multicast_u (batch2454) and gj_ipv6_is_multicast
 * (batch1815) — unique is_ipv4_*_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2836_marker[] = "libcgj-batch2836";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if top 4 bits are 1110 (224.0.0.0/4). */
static int
b2836_multicast_p(uint32_t u32Addr)
{
	if ((u32Addr >> 28) == 0xeu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_ipv4_multicast_u - 1 iff host-order IPv4 is multicast (224/4).
 *
 * addr: IPv4 as host-order uint32 (e.g. 224.0.0.1 -> 0xe0000001).
 * Covers 224.0.0.0 .. 239.255.255.255 inclusive. Does not call libc.
 */
int
gj_is_ipv4_multicast_u(uint32_t u32Addr)
{
	(void)NULL;
	return b2836_multicast_p(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_ipv4_multicast_u(uint32_t u32Addr)
    __attribute__((alias("gj_is_ipv4_multicast_u")));
