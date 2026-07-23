/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2455: host-order IPv4 limited-broadcast predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_is_broadcast_u(uint32_t addr);
 *     - 1 if host-order addr is 255.255.255.255 (limited broadcast), else 0.
 *   int __gj_ipv4_is_broadcast_u  (alias)
 *   __libcgj_batch2455_marker = "libcgj-batch2455"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_ether_is_broadcast
 * (batch1107) and directed subnet broadcast helpers. Unique surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2455_marker[] = "libcgj-batch2455";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if addr is the limited broadcast all-ones address. */
static int
b2455_broadcast_p(uint32_t u32Addr)
{
	if (u32Addr == 0xffffffffu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_is_broadcast_u - 1 iff host-order IPv4 is 255.255.255.255.
 *
 * addr: IPv4 as host-order uint32.
 * Tests limited broadcast only (not directed subnet broadcasts).
 * Does not call libc.
 */
int
gj_ipv4_is_broadcast_u(uint32_t u32Addr)
{
	(void)NULL;
	return b2455_broadcast_p(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_is_broadcast_u(uint32_t u32Addr)
    __attribute__((alias("gj_ipv4_is_broadcast_u")));
