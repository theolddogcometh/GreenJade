/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2452: host-order IPv4 private (RFC1918) predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_is_private_u(uint32_t addr);
 *     - 1 if host-order addr is RFC 1918 private, else 0:
 *         10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16.
 *   int __gj_ipv4_is_private_u  (alias)
 *   __libcgj_batch2452_marker = "libcgj-batch2452"
 *
 * Net exclusive wave (2451-2460). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2452_marker[] = "libcgj-batch2452";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if host-order addr is in any RFC 1918 private block. */
static int
b2452_private_p(uint32_t u32Addr)
{
	/* 10.0.0.0/8 */
	if ((u32Addr >> 24) == 0x0au) {
		return 1;
	}
	/* 172.16.0.0/12  (172.16.0.0 .. 172.31.255.255) */
	if ((u32Addr >> 20) == 0xac1u) {
		return 1;
	}
	/* 192.168.0.0/16 */
	if ((u32Addr >> 16) == 0xc0a8u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_is_private_u - 1 iff host-order IPv4 is RFC 1918 private.
 *
 * addr: IPv4 as host-order uint32.
 * Covers 10/8, 172.16/12, and 192.168/16 only (not CGNAT 100.64/10).
 * Does not call libc.
 */
int
gj_ipv4_is_private_u(uint32_t u32Addr)
{
	(void)NULL;
	return b2452_private_p(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_is_private_u(uint32_t u32Addr)
    __attribute__((alias("gj_ipv4_is_private_u")));
