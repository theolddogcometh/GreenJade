/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1813: IPv6 link-local predicate (fe80::/10).
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_linklocal_p(const uint8_t addr[16]);
 *     — Return 1 if addr is in fe80::/10 (RFC 4291), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_linklocal_p  (alias)
 *   __libcgj_batch1813_marker = "libcgj-batch1813"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1813_marker[] = "libcgj-batch1813";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if the first 10 bits of aAddr form 1111111010 (fe80::/10).
 * Network-order: addr[0] == 0xfe and (addr[1] & 0xc0) == 0x80.
 */
static int
b1813_fe80_prefix10(const uint8_t aAddr[16])
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
 * gj_ipv6_linklocal_p — 1 if first 10 bits are 1111111010 (fe80::/10).
 *
 * Includes the full fe80::/10 block (not only fe80::/64).
 * Edge cases:
 *   addr == NULL → 0
 *   fec0::/10 (site-local) → 0
 *   ff80::/10 (multicast) → 0
 */
int
gj_ipv6_linklocal_p(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b1813_fe80_prefix10(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_linklocal_p(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_linklocal_p")));
