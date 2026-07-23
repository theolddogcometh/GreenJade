/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3385: Ethernet unicast MAC predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_mac_is_unicast_u(const uint8_t mac[6]);
 *     - Return 1 if the I/G bit (bit 0 of the first octet) is clear
 *       (individual/unicast address), else 0. mac == NULL -> 0.
 *   int __gj_mac_is_unicast_u  (alias)
 *   __libcgj_batch3385_marker = "libcgj-batch3385"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_mac_is_mcast
 * (batch799) — unique unicast _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3385_marker[] = "libcgj-batch3385";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if I/G bit (LSB of first octet) is clear. */
static int
b3385_unicast_p(const uint8_t aMac[6])
{
	return ((aMac[0] & 0x01u) == 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_is_unicast_u - 1 iff MAC is individual (I/G bit clear).
 *
 * IEEE 802: least-significant bit of the first octet is the I/G bit.
 * Does not call libc. No parent wires.
 */
int
gj_mac_is_unicast_u(const uint8_t aMac[6])
{
	if (aMac == NULL) {
		return 0;
	}
	return b3385_unicast_p(aMac);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mac_is_unicast_u(const uint8_t aMac[6])
    __attribute__((alias("gj_mac_is_unicast_u")));
