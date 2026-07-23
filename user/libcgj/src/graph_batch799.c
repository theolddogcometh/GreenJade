/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch799: Ethernet multicast MAC predicate.
 *
 * Surface (unique symbols):
 *   int gj_mac_is_mcast(const unsigned char mac[6]);
 *     — Return 1 if the I/G bit (bit 0 of the first octet) is set,
 *       else 0. mac == NULL → 0.
 *   int __gj_mac_is_mcast  (alias)
 *   __libcgj_batch799_marker = "libcgj-batch799"
 *
 * Distinct from gj_mac_is_zero (batch798) / gj_mac_parse (batch157) —
 * unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch799_marker[] = "libcgj-batch799";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_is_mcast — 1 if bit0 of first octet is set (multicast/group).
 *
 * IEEE 802: least-significant bit of the first octet is the I/G bit.
 * Broadcast FF:FF:FF:FF:FF:FF is also mcast under this test.
 */
int
gj_mac_is_mcast(const unsigned char aMac[6])
{
	if (aMac == NULL) {
		return 0;
	}
	return ((aMac[0] & 0x01u) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mac_is_mcast(const unsigned char aMac[6])
    __attribute__((alias("gj_mac_is_mcast")));
