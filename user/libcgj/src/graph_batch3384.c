/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3384: IPv6 multicast predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_multicast_u(const uint8_t addr[16]);
 *     - Return 1 if addr is in ff00::/8 (RFC 4291), else 0.
 *       addr == NULL -> 0.
 *   int __gj_ipv6_is_multicast_u  (alias)
 *   __libcgj_batch3384_marker = "libcgj-batch3384"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_ipv6_is_multicast
 * (batch1815) and gj_ipv4_is_multicast_u (batch2454) — unique _u
 * IPv6 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3384_marker[] = "libcgj-batch3384";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if high octet is 0xff (ff00::/8). */
static int
b3384_ff00_prefix8(const uint8_t aAddr[16])
{
	return (aAddr[0] == 0xffu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_multicast_u - 1 iff addr is in ff00::/8 (RFC 4291).
 *
 * Matches all multicast scopes (interface-local through global).
 * Does not call libc. No parent wires.
 */
int
gj_ipv6_is_multicast_u(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b3384_ff00_prefix8(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_multicast_u(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_multicast_u")));
