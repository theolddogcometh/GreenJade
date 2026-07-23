/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3387: Ethernet broadcast MAC predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_mac_is_broadcast_u(const uint8_t mac[6]);
 *     - Return 1 if all six octets are 0xff, else 0.
 *       mac == NULL -> 0.
 *   int __gj_mac_is_broadcast_u  (alias)
 *   __libcgj_batch3387_marker = "libcgj-batch3387"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_ether_is_broadcast
 * (batch1107) — unique mac_is_broadcast_u surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3387_marker[] = "libcgj-batch3387";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all six octets are 0xff. */
static int
b3387_broadcast_p(const uint8_t aMac[6])
{
	unsigned i;

	for (i = 0u; i < 6u; i++) {
		if (aMac[i] != 0xffu) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_is_broadcast_u - 1 iff mac is ff:ff:ff:ff:ff:ff.
 *
 * Does not call libc. No parent wires.
 */
int
gj_mac_is_broadcast_u(const uint8_t aMac[6])
{
	if (aMac == NULL) {
		return 0;
	}
	return b3387_broadcast_p(aMac);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mac_is_broadcast_u(const uint8_t aMac[6])
    __attribute__((alias("gj_mac_is_broadcast_u")));
