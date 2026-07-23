/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3383: IPv6 link-local predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_linklocal_u(const uint8_t addr[16]);
 *     - Return 1 if addr is in fe80::/10 (RFC 4291), else 0.
 *       addr == NULL -> 0.
 *   int __gj_ipv6_is_linklocal_u  (alias)
 *   __libcgj_batch3383_marker = "libcgj-batch3383"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_ipv6_is_linklocal
 * (batch1113) — unique _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3383_marker[] = "libcgj-batch3383";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if first 10 bits are 1111111010 (fe80::/10).
 * Network-order: addr[0] == 0xfe and (addr[1] & 0xc0) == 0x80.
 */
static int
b3383_linklocal_p(const uint8_t aAddr[16])
{
	if (aAddr[0] != 0xfeu) {
		return 0;
	}
	if ((aAddr[1] & 0xc0u) != 0x80u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_linklocal_u - 1 iff addr is in fe80::/10 (RFC 4291).
 *
 * Does not call libc. No parent wires.
 */
int
gj_ipv6_is_linklocal_u(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b3383_linklocal_p(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_linklocal_u(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_linklocal_u")));
