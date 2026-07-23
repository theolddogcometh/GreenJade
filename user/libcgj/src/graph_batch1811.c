/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1811: IPv6 unspecified-address predicate.
 *
 * Surface (unique symbols for this TU):
 *   int gj_ipv6_unspecified_p(const uint8_t addr[16]);
 *     — Return 1 if addr is :: (all 16 octets zero), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_unspecified_p  (alias)
 *   __libcgj_batch1811_marker = "libcgj-batch1811"
 *
 * IPv6 helpers exclusive continuum 1811..1820. Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1811_marker[] = "libcgj-batch1811";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all 16 octets of aAddr are zero. */
static int
b1811_all_zero(const uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_unspecified_p — 1 if addr is :: (RFC 4291).
 *
 * Edge cases:
 *   addr == NULL → 0
 *   any non-zero octet → 0
 */
int
gj_ipv6_unspecified_p(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b1811_all_zero(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_unspecified_p(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_unspecified_p")));
