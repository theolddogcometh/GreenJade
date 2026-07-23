/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11567: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_11567(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_11567  (alias)
 *   __libcgj_batch11567_marker = "libcgj-batch11567"
 *
 * Net/connectivity soft product CREATE-ONLY (11561-11570). Unique
 * gj_steam_net_ok_u_11567 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_11367 (batch11367), gj_steam_net_ok_u_11167
 * (batch11167), gj_net_ok_u_11561, gj_wifi_ok_u_11562,
 * gj_ethernet_ok_u_11563, gj_dns_ok_u_11564, gj_https_ok_u_11565,
 * gj_cdn_ok_u_11566, gj_offline_mode_ok_u_11568,
 * gj_net_soft_ready_u_11569, and gj_batch_id_11570. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11567_marker[] = "libcgj-batch11567";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B11567_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11567_steam_net_ok(void)
{
	return B11567_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_11567 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_11567(void)
{
	(void)NULL;
	return b11567_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_11567(void)
    __attribute__((alias("gj_steam_net_ok_u_11567")));
