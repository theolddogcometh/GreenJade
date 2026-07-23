/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1812: IPv6 loopback-address predicate.
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_loopback_p(const uint8_t addr[16]);
 *     — Return 1 if addr is ::1, else 0. addr == NULL → 0.
 *   int __gj_ipv6_loopback_p  (alias)
 *   __libcgj_batch1812_marker = "libcgj-batch1812"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1812_marker[] = "libcgj-batch1812";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if first 15 octets are zero. */
static int
b1812_prefix15_zero(const uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 15u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_loopback_p — 1 if addr is ::1 (RFC 4291).
 *
 * Network-order: octets 0..14 == 0 and octet 15 == 1.
 * Edge cases:
 *   addr == NULL → 0
 *   ::0, ::2, etc. → 0
 */
int
gj_ipv6_loopback_p(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	if (!b1812_prefix15_zero(aAddr)) {
		return 0;
	}
	return (aAddr[15] == 1u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_loopback_p(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_loopback_p")));
