/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5070: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5070(void);
 *     - Returns the compile-time graph batch number for this TU (5070).
 *   uint32_t __gj_batch_id_5070  (alias)
 *   __libcgj_batch5070_marker = "libcgj-batch5070"
 *
 * Exclusive continuum CREATE-ONLY (5061-5070: net_eth_alen_u,
 * net_ip4_alen_u, net_ip6_alen_u, net_eth_is_mcast_u, net_eth_is_zero_u,
 * net_eth_eq_u, net_htons_u, net_ntohs_u, net_checksum_fold_u,
 * batch_id_5070). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5070_marker[] = "libcgj-batch5070";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5070_id(void)
{
	return 5070u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5070 - report this TU's graph batch number.
 *
 * Always returns 5070. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5070(void)
{
	(void)NULL;
	return b5070_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5070(void)
    __attribute__((alias("gj_batch_id_5070")));
