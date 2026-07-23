/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1113: IPv6 link-local predicate (fe80::/10).
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_linklocal(const uint8_t addr[16]);
 *     — Return 1 if addr is in fe80::/10 (RFC 4291), else 0.
 *       addr == NULL → 0.
 *   int __gj_ipv6_is_linklocal  (alias)
 *   __libcgj_batch1113_marker = "libcgj-batch1113"
 *
 * Distinct from gj_ipv6_is_loopback / is_unspecified — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1113_marker[] = "libcgj-batch1113";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_linklocal — 1 if first 10 bits are 1111111010 (fe80::/10).
 *
 * Network-order bytes: addr[0] == 0xfe and (addr[1] & 0xc0) == 0x80.
 */
int
gj_ipv6_is_linklocal(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}

	if (aAddr[0] != 0xfeu) {
		return 0;
	}
	if ((aAddr[1] & 0xc0u) != 0x80u) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_linklocal(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_linklocal")));
