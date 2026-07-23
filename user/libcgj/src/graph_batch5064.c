/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5064: Ethernet multicast MAC predicate.
 *
 * Surface (unique symbols):
 *   int gj_net_eth_is_mcast_u(const uint8_t *m);
 *     - Return 1 if the I/G bit (bit 0 of the first octet) is set,
 *       else 0. m == NULL → 0.
 *   int __gj_net_eth_is_mcast_u  (alias)
 *   __libcgj_batch5064_marker = "libcgj-batch5064"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_mac_is_mcast (batch799) — unique gj_net_eth_is_mcast_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5064_marker[] = "libcgj-batch5064";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * IEEE 802: least-significant bit of the first octet is the I/G bit.
 * Broadcast FF:FF:FF:FF:FF:FF is also mcast under this test.
 */
static int
b5064_is_mcast(const uint8_t *pM)
{
	if (pM == NULL) {
		return 0;
	}
	return ((pM[0] & 0x01u) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_eth_is_mcast_u - 1 if bit0 of first octet is set (multicast).
 *
 * m: pointer to at least 1 (typically 6) MAC octet(s); NULL → 0
 *
 * Self-contained; no parent wires.
 */
int
gj_net_eth_is_mcast_u(const uint8_t *m)
{
	(void)NULL;
	return b5064_is_mcast(m);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_eth_is_mcast_u(const uint8_t *m)
    __attribute__((alias("gj_net_eth_is_mcast_u")));
