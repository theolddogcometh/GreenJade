/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13261: net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_13261(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       network connectivity probe for the net soft continuum.
 *   uint32_t __gj_net_ok_u_13261  (alias)
 *   __libcgj_batch13261_marker = "libcgj-batch13261"
 *
 * Exclusive continuum CREATE-ONLY (13261-13270: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13270 —
 * net_ok_u_13261, wifi_ok_u_13262, ethernet_ok_u_13263,
 * dns_ok_u_13264, https_ok_u_13265, cdn_ok_u_13266,
 * steam_net_ok_u_13267, offline_mode_ok_u_13268,
 * net_soft_ready_u_13269, batch_id_13270). Unique surface only; no
 * multi-def. Distinct from gj_net_ok_u_13061 (batch13061),
 * gj_net_ok_u_12861 (batch12861), gj_net_ok_u_12661 (batch12661),
 * gj_net_ok_u_12461 (batch12461), and sibling net soft ok_u stubs in
 * this wave. No parent wires. No __int128. No net implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13261_marker[] = "libcgj-batch13261";

/* Soft net-ok lamp: always off (not a real net probe). */
#define B13261_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13261_net_ok(void)
{
	return B13261_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_13261 - net connectivity soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe interfaces or
 * call libc. No parent wires.
 */
uint32_t
gj_net_ok_u_13261(void)
{
	(void)NULL;
	return b13261_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_13261(void)
    __attribute__((alias("gj_net_ok_u_13261")));
