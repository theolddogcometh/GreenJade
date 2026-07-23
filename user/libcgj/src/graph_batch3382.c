/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3382: IPv6 loopback-address predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_loopback_u(const uint8_t addr[16]);
 *     - Return 1 if addr is ::1, else 0. addr == NULL -> 0.
 *   int __gj_ipv6_is_loopback_u  (alias)
 *   __libcgj_batch3382_marker = "libcgj-batch3382"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_ipv6_is_loopback
 * (batch1112) — unique _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3382_marker[] = "libcgj-batch3382";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if addr is 00..00 01 (::1). */
static int
b3382_loopback_p(const uint8_t aAddr[16])
{
	unsigned i;

	for (i = 0u; i < 15u; i++) {
		if (aAddr[i] != 0u) {
			return 0;
		}
	}
	return (aAddr[15] == 1u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv6_is_loopback_u - 1 iff addr is ::1 (RFC 4291).
 *
 * Does not call libc. No parent wires.
 */
int
gj_ipv6_is_loopback_u(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b3382_loopback_p(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_loopback_u(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_loopback_u")));
