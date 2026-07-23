/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13663: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_13663(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ethernet/NIC probe for the net soft continuum.
 *   uint32_t __gj_ethernet_ok_u_13663  (alias)
 *   __libcgj_batch13663_marker = "libcgj-batch13663"
 *
 * Exclusive continuum CREATE-ONLY (13661-13670: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13670 —
 * net_ok_u_13661, wifi_ok_u_13662, ethernet_ok_u_13663,
 * dns_ok_u_13664, https_ok_u_13665, cdn_ok_u_13666,
 * steam_net_ok_u_13667, offline_mode_ok_u_13668,
 * net_soft_ready_u_13669, batch_id_13670). Unique surface only; no
 * multi-def. Distinct from gj_ethernet_ok_u_13463 (batch13463),
 * gj_ethernet_ok_u_13263 (batch13263), gj_ethernet_ok_u_13063
 * (batch13063), gj_ethernet_ok_u_12863 (batch12863), and sibling net
 * soft ok_u stubs in this wave. No parent wires. No __int128. No
 * ethernet implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13663_marker[] = "libcgj-batch13663";

/* Soft ethernet-ok lamp: always off (not a real NIC probe). */
#define B13663_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13663_ethernet_ok(void)
{
	return B13663_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_13663 - ethernet connectivity soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe NICs or call
 * libc. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_13663(void)
{
	(void)NULL;
	return b13663_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_13663(void)
    __attribute__((alias("gj_ethernet_ok_u_13663")));
