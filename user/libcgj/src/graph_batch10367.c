/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10367: network soft steam-net-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_10367(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       Steam network/CDN probe for the net soft continuum.
 *   uint32_t __gj_steam_net_ok_u_10367  (alias)
 *   __libcgj_batch10367_marker = "libcgj-batch10367"
 *
 * Exclusive continuum CREATE-ONLY (10361-10370: net soft all→0 —
 * net_ok_u_10361, wifi_ok_u_10362, ethernet_ok_u_10363, dns_ok_u_10364,
 * https_ok_u_10365, cdn_ok_u_10366, steam_net_ok_u_10367,
 * offline_mode_ok_u_10368, net_soft_ready_u_10369, batch_id_10370).
 * Unique surface only; no multi-def. Distinct from
 * gj_steam_cdn_ok_u_10068. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10367_marker[] = "libcgj-batch10367";

/* Soft steam-net-ok lamp: always off (not a real Steam net probe). */
#define B10367_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10367_steam_net_ok(void)
{
	return B10367_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_10367 - network soft steam-net-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not contact Steam or
 * call libc. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_10367(void)
{
	(void)NULL;
	return b10367_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_10367(void)
    __attribute__((alias("gj_steam_net_ok_u_10367")));
