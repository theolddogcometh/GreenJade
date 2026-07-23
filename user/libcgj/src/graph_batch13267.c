/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13267: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_13267(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam networking probe for the net soft continuum.
 *   uint32_t __gj_steam_net_ok_u_13267  (alias)
 *   __libcgj_batch13267_marker = "libcgj-batch13267"
 *
 * Exclusive continuum CREATE-ONLY (13261-13270: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13270 —
 * net_ok_u_13261, wifi_ok_u_13262, ethernet_ok_u_13263,
 * dns_ok_u_13264, https_ok_u_13265, cdn_ok_u_13266,
 * steam_net_ok_u_13267, offline_mode_ok_u_13268,
 * net_soft_ready_u_13269, batch_id_13270). Unique surface only; no
 * multi-def. Distinct from gj_steam_net_ok_u_13067 (batch13067),
 * gj_steam_net_ok_u_12867 (batch12867), gj_steam_net_ok_u_12667
 * (batch12667), gj_steam_net_ok_u_12467 (batch12467), and sibling net
 * soft ok_u stubs in this wave. No parent wires. No __int128. No Steam
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13267_marker[] = "libcgj-batch13267";

/* Soft steam-net-ok lamp: always off (not a real Steam probe). */
#define B13267_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13267_steam_net_ok(void)
{
	return B13267_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_13267 - Steam networking soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not contact Steam or call
 * libc. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_13267(void)
{
	(void)NULL;
	return b13267_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_13267(void)
    __attribute__((alias("gj_steam_net_ok_u_13267")));
