/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5061: Ethernet address length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_eth_alen_u(void);
 *     - Returns the canonical Ethernet MAC address length in octets
 *       (ETH_ALEN = 6).
 *   uint32_t __gj_net_eth_alen_u  (alias)
 *   __libcgj_batch5061_marker = "libcgj-batch5061"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070: net_eth_alen_u,
 * net_ip4_alen_u, net_ip6_alen_u, net_eth_is_mcast_u, net_eth_is_zero_u,
 * net_eth_eq_u, net_htons_u, net_ntohs_u, net_checksum_fold_u,
 * batch_id_5070). Distinct from gj_mac_is_mcast (batch799) /
 * gj_mac_parse (batch157) — unique gj_net_eth_alen_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5061_marker[] = "libcgj-batch5061";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5061_eth_alen(void)
{
	return 6u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_eth_alen_u - Ethernet MAC address length in octets.
 *
 * Always returns 6 (ETH_ALEN). Self-contained; no parent wires.
 */
uint32_t
gj_net_eth_alen_u(void)
{
	(void)NULL;
	return b5061_eth_alen();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_eth_alen_u(void)
    __attribute__((alias("gj_net_eth_alen_u")));
