/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11167: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_11167(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_11167  (alias)
 *   __libcgj_batch11167_marker = "libcgj-batch11167"
 *
 * Net/connectivity soft product CREATE-ONLY (11161-11170). Unique
 * gj_steam_net_ok_u_11167 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_10967 (batch10967), gj_net_ok_u_11161,
 * gj_wifi_ok_u_11162, gj_ethernet_ok_u_11163, gj_dns_ok_u_11164,
 * gj_https_ok_u_11165, gj_cdn_ok_u_11166, gj_offline_mode_ok_u_11168,
 * gj_net_soft_ready_u_11169, and gj_batch_id_11170. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11167_marker[] = "libcgj-batch11167";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B11167_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11167_steam_net_ok(void)
{
	return B11167_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_11167 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_11167(void)
{
	(void)NULL;
	return b11167_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_11167(void)
    __attribute__((alias("gj_steam_net_ok_u_11167")));
