/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10567: steam net ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_10567(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam network/content-server runtime probe for the net soft path.
 *   uint32_t __gj_steam_net_ok_u_10567  (alias)
 *   __libcgj_batch10567_marker = "libcgj-batch10567"
 *
 * Exclusive continuum CREATE-ONLY (10561-10570: net soft id stubs —
 * net_ok_u_10561, wifi_ok_u_10562, ethernet_ok_u_10563, dns_ok_u_10564,
 * https_ok_u_10565, cdn_ok_u_10566, steam_net_ok_u_10567,
 * offline_mode_ok_u_10568, net_soft_ready_u_10569, batch_id_10570).
 * Unique surface only; no multi-def. Distinct from
 * gj_steam_cdn_ok_u_10068. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10567_marker[] = "libcgj-batch10567";

/* Soft steam-net-ok lamp: always off (not a real runtime probe). */
#define B10567_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10567_ok(void)
{
	return B10567_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_10567 - steam net ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Steam network
 * or content servers or call libc. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_10567(void)
{
	(void)NULL;
	return b10567_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_10567(void)
    __attribute__((alias("gj_steam_net_ok_u_10567")));
