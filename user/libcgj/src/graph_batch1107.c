/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1107: Ethernet broadcast MAC predicate.
 *
 * Surface (unique symbols):
 *   int gj_ether_is_broadcast(const unsigned char mac[6]);
 *     — Return 1 if all six octets are 0xff, else 0.
 *       mac == NULL → 0.
 *   int __gj_ether_is_broadcast  (alias)
 *   __libcgj_batch1107_marker = "libcgj-batch1107"
 *
 * Distinct from gj_mac_is_mcast (batch799) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1107_marker[] = "libcgj-batch1107";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ether_is_broadcast — 1 if mac is ff:ff:ff:ff:ff:ff.
 */
int
gj_ether_is_broadcast(const unsigned char aMac[6])
{
	unsigned i;

	if (aMac == NULL) {
		return 0;
	}

	for (i = 0u; i < 6u; i++) {
		if (aMac[i] != 0xffu) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ether_is_broadcast(const unsigned char aMac[6])
    __attribute__((alias("gj_ether_is_broadcast")));
