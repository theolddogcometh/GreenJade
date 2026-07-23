/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1112: IPv6 loopback-address predicate.
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_loopback(const uint8_t addr[16]);
 *     — Return 1 if addr is ::1, else 0. addr == NULL → 0.
 *   int __gj_ipv6_is_loopback  (alias)
 *   __libcgj_batch1112_marker = "libcgj-batch1112"
 *
 * Distinct from gj_ipv6_is_unspecified (batch1111) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1112_marker[] = "libcgj-batch1112";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_loopback — 1 if addr is ::1 (RFC 4291).
 */
int
gj_ipv6_is_loopback(const uint8_t aAddr[16])
{
	unsigned i;

	if (aAddr == NULL) {
		return 0;
	}

	for (i = 0u; i < 15u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	return (aAddr[15] == 1u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_loopback(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_loopback")));
