/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1816: IPv4-mapped IPv6 address predicate.
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_v4mapped_p(const uint8_t addr[16]);
 *     — Return 1 if addr is ::ffff:0:0/96 (IPv4-mapped), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_v4mapped_p  (alias)
 *   __libcgj_batch1816_marker = "libcgj-batch1816"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1816_marker[] = "libcgj-batch1816";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if aAddr matches ::ffff:0:0/96 prefix:
 *   octets 0..9 == 0, octets 10..11 == 0xff 0xff.
 * Last 4 octets (embedded IPv4) are not inspected.
 */
static int
b1816_v4mapped_prefix96(const uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 10u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	if (aAddr[10] != 0xffu || aAddr[11] != 0xffu) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_v4mapped_p — 1 if addr is ::ffff:a.b.c.d (RFC 4291).
 *
 * Edge cases:
 *   addr == NULL → 0
 *   ::ffff:0.0.0.0 through ::ffff:255.255.255.255 → 1
 *   IPv4-compatible ::a.b.c.d (no ffff) → 0
 */
int
gj_ipv6_v4mapped_p(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b1816_v4mapped_prefix96(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_v4mapped_p(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_v4mapped_p")));
