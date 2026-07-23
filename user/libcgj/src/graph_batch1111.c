/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1111: IPv6 unspecified-address predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_unspecified(const uint8_t addr[16]);
 *     — Return 1 if addr is :: (all 16 octets zero), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_is_unspecified  (alias)
 *   __libcgj_batch1111_marker = "libcgj-batch1111"
 *
 * Distinct from gj_ipv6_in_range (batch541) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1111_marker[] = "libcgj-batch1111";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_unspecified — 1 if addr is :: (RFC 4291).
 */
int
gj_ipv6_is_unspecified(const uint8_t aAddr[16])
{
	unsigned i;

	if (aAddr == NULL) {
		return 0;
	}

	for (i = 0u; i < 16u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_unspecified(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_unspecified")));
