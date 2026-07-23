/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5063: IPv6 address length constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ip6_alen_u(void);
 *     - Returns the canonical IPv6 address length in octets
 *       (IN6_ADDR_SIZE = 16).
 *   uint32_t __gj_net_ip6_alen_u  (alias)
 *   __libcgj_batch5063_marker = "libcgj-batch5063"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070). Distinct from
 * gj_net_eth_alen_u (batch5061) / gj_net_ip4_alen_u (batch5062) —
 * unique gj_net_ip6_alen_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5063_marker[] = "libcgj-batch5063";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5063_ip6_alen(void)
{
	return 16u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ip6_alen_u - IPv6 address length in octets.
 *
 * Always returns 16. Self-contained; no parent wires.
 */
uint32_t
gj_net_ip6_alen_u(void)
{
	(void)NULL;
	return b5063_ip6_alen();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ip6_alen_u(void)
    __attribute__((alias("gj_net_ip6_alen_u")));
