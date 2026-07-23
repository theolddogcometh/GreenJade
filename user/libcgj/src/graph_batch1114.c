/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1114: IPv4-mapped IPv6 address predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_v4mapped(const uint8_t addr[16]);
 *     — Return 1 if addr is ::ffff:0:0/96 (IPv4-mapped), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_is_v4mapped  (alias)
 *   __libcgj_batch1114_marker = "libcgj-batch1114"
 *
 * Distinct from other gj_ipv6_is_* helpers — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1114_marker[] = "libcgj-batch1114";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_v4mapped — 1 if addr is ::ffff:a.b.c.d (RFC 4291).
 *
 * First 10 octets zero, octets 10..11 are 0xff 0xff; last 4 are IPv4.
 */
int
gj_ipv6_is_v4mapped(const uint8_t aAddr[16])
{
	unsigned i;

	if (aAddr == NULL) {
		return 0;
	}

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

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_v4mapped(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_v4mapped")));
