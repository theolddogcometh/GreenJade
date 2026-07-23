/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13067: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_13067(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_13067  (alias)
 *   __libcgj_batch13067_marker = "libcgj-batch13067"
 *
 * Net/connectivity soft product CREATE-ONLY (13061-13070). Unique
 * gj_steam_net_ok_u_13067 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_12867 (batch12867), gj_steam_net_ok_u_12667
 * (batch12667), gj_steam_net_ok_u_12467 (batch12467),
 * gj_steam_net_ok_u_12267 (batch12267), gj_net_ok_u_13061,
 * gj_wifi_ok_u_13062, gj_ethernet_ok_u_13063, gj_dns_ok_u_13064,
 * gj_https_ok_u_13065, gj_cdn_ok_u_13066,
 * gj_offline_mode_ok_u_13068, gj_net_soft_ready_u_13069, and
 * gj_batch_id_13070. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13067_marker[] = "libcgj-batch13067";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B13067_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13067_steam_net_ok(void)
{
	return B13067_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_13067 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_13067(void)
{
	(void)NULL;
	return b13067_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_13067(void)
    __attribute__((alias("gj_steam_net_ok_u_13067")));
