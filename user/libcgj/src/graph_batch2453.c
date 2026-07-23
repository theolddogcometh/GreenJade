/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2453: host-order IPv4 link-local predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv4_is_linklocal_u(uint32_t addr);
 *     - 1 if host-order addr is in 169.254.0.0/16 (RFC 3927), else 0.
 *   int __gj_ipv4_is_linklocal_u  (alias)
 *   __libcgj_batch2453_marker = "libcgj-batch2453"
 *
 * Net exclusive wave (2451-2460). Distinct from gj_ipv6_is_linklocal
 * (batch1113). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2453_marker[] = "libcgj-batch2453";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if top 16 bits are 169.254 (0xa9fe). */
static int
b2453_linklocal_p(uint32_t u32Addr)
{
	if ((u32Addr >> 16) == 0xa9feu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_is_linklocal_u - 1 iff host-order IPv4 is 169.254.0.0/16.
 *
 * addr: IPv4 as host-order uint32 (e.g. 169.254.1.1 -> 0xa9fe0101).
 * Does not call libc.
 */
int
gj_ipv4_is_linklocal_u(uint32_t u32Addr)
{
	(void)NULL;
	return b2453_linklocal_p(u32Addr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_is_linklocal_u(uint32_t u32Addr)
    __attribute__((alias("gj_ipv4_is_linklocal_u")));
