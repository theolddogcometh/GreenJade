/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2834: host-order IPv4 loopback predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_is_ipv4_loopback_u(uint32_t addr);
 *     - 1 if host-order addr is in 127.0.0.0/8, else 0.
 *   int __gj_is_ipv4_loopback_u  (alias)
 *   __libcgj_batch2834_marker = "libcgj-batch2834"
 *
 * IPv4/endian exclusive wave (2831-2840). Distinct from
 * gj_ipv4_is_loopback (batch859) and gj_ipv4_is_loopback_u (batch2451)
 * — unique is_ipv4_*_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2834_marker[] = "libcgj-batch2834";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if high octet is 127 (host-order 127.0.0.0/8). */
static int
b2834_loopback_p(uint32_t u32Addr)
{
	if ((u32Addr >> 24) == 0x7fu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_ipv4_loopback_u - 1 iff host-order IPv4 is in 127.0.0.0/8.
 *
 * addr: IPv4 as host-order uint32 (e.g. 127.0.0.1 -> 0x7f000001).
 * Includes the full 127/8 block. Does not call libc.
 */
int
gj_is_ipv4_loopback_u(uint32_t u32Addr)
{
	(void)NULL;
	return b2834_loopback_p(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_ipv4_loopback_u(uint32_t u32Addr)
    __attribute__((alias("gj_is_ipv4_loopback_u")));
