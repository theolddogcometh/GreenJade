/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12867: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_12867(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_12867  (alias)
 *   __libcgj_batch12867_marker = "libcgj-batch12867"
 *
 * Net/connectivity soft product CREATE-ONLY (12861-12870). Unique
 * gj_steam_net_ok_u_12867 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_12667 (batch12667), gj_steam_net_ok_u_12467
 * (batch12467), gj_steam_net_ok_u_12267 (batch12267),
 * gj_net_ok_u_12861, gj_wifi_ok_u_12862, gj_ethernet_ok_u_12863,
 * gj_dns_ok_u_12864, gj_https_ok_u_12865, gj_cdn_ok_u_12866,
 * gj_offline_mode_ok_u_12868, gj_net_soft_ready_u_12869, and
 * gj_batch_id_12870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12867_marker[] = "libcgj-batch12867";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B12867_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12867_steam_net_ok(void)
{
	return B12867_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_12867 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_12867(void)
{
	(void)NULL;
	return b12867_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_12867(void)
    __attribute__((alias("gj_steam_net_ok_u_12867")));
