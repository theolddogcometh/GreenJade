/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12067: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_12067(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_12067  (alias)
 *   __libcgj_batch12067_marker = "libcgj-batch12067"
 *
 * Net/connectivity soft product CREATE-ONLY (12061-12070). Unique
 * gj_steam_net_ok_u_12067 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_11967 (batch11967), gj_steam_net_ok_u_11767
 * (batch11767), gj_net_ok_u_12061, gj_wifi_ok_u_12062,
 * gj_ethernet_ok_u_12063, gj_dns_ok_u_12064, gj_https_ok_u_12065,
 * gj_cdn_ok_u_12066, gj_offline_mode_ok_u_12068,
 * gj_net_soft_ready_u_12069, and gj_batch_id_12070. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12067_marker[] = "libcgj-batch12067";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B12067_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12067_steam_net_ok(void)
{
	return B12067_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_12067 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_12067(void)
{
	(void)NULL;
	return b12067_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_12067(void)
    __attribute__((alias("gj_steam_net_ok_u_12067")));
