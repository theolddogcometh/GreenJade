/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1815: IPv6 multicast predicate (ff00::/8).
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_is_multicast(const uint8_t addr[16]);
 *     — Return 1 if addr is in ff00::/8 (RFC 4291), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_is_multicast  (alias)
 *   __libcgj_batch1815_marker = "libcgj-batch1815"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1815_marker[] = "libcgj-batch1815";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if high octet is 0xff (ff00::/8). */
static int
b1815_ff00_prefix8(const uint8_t aAddr[16])
{
	return (aAddr[0] == 0xffu) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_multicast — 1 if addr is in ff00::/8 (RFC 4291).
 *
 * Matches all multicast scopes (interface-local through global).
 * Edge cases:
 *   addr == NULL → 0
 *   fe80::/10, ::1, unicast → 0
 */
int
gj_ipv6_is_multicast(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b1815_ff00_prefix8(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_multicast(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_multicast")));
