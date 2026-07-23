/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3381: IPv6 unspecified-address predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ipv6_is_unspec_u(const uint8_t addr[16]);
 *     - Return 1 if addr is :: (all 16 octets zero), else 0.
 *       addr == NULL -> 0.
 *   int __gj_ipv6_is_unspec_u  (alias)
 *   __libcgj_batch3381_marker = "libcgj-batch3381"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_ipv6_is_unspecified
 * (batch1111) — unique _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3381_marker[] = "libcgj-batch3381";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all 16 octets are zero. */
static int
b3381_all_zero(const uint8_t aAddr[16])
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
 * gj_ipv6_is_unspec_u - 1 iff addr is :: (RFC 4291 unspecified).
 *
 * Does not call libc. No parent wires.
 */
int
gj_ipv6_is_unspec_u(const uint8_t aAddr[16])
{
	if (aAddr == NULL) {
		return 0;
	}
	return b3381_all_zero(aAddr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv6_is_unspec_u(const uint8_t aAddr[16])
    __attribute__((alias("gj_ipv6_is_unspec_u")));
