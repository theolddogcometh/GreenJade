/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10361: network soft net-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_10361(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       network connectivity probe for the net soft continuum.
 *   uint32_t __gj_net_ok_u_10361  (alias)
 *   __libcgj_batch10361_marker = "libcgj-batch10361"
 *
 * Exclusive continuum CREATE-ONLY (10361-10370: net soft all→0 —
 * net_ok_u_10361, wifi_ok_u_10362, ethernet_ok_u_10363, dns_ok_u_10364,
 * https_ok_u_10365, cdn_ok_u_10366, steam_net_ok_u_10367,
 * offline_mode_ok_u_10368, net_soft_ready_u_10369, batch_id_10370).
 * Unique surface only; no multi-def. Distinct from prior net/wifi
 * soft lamps (e.g. gj_wifi_ok_u_10061). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10361_marker[] = "libcgj-batch10361";

/* Soft net-ok lamp: always off (not a real connectivity probe). */
#define B10361_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10361_net_ok(void)
{
	return B10361_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_10361 - network soft net-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe interfaces or
 * call libc. No parent wires.
 */
uint32_t
gj_net_ok_u_10361(void)
{
	(void)NULL;
	return b10361_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_10361(void)
    __attribute__((alias("gj_net_ok_u_10361")));
