/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10267: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_10267(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or services.
 *   uint32_t __gj_steam_net_ok_u_10267  (alias)
 *   __libcgj_batch10267_marker = "libcgj-batch10267"
 *
 * Net/connectivity soft product CREATE-ONLY (10261-10270). Unique
 * gj_steam_net_ok_u_10267 surface only; no multi-def. Distinct from
 * gj_net_ok_u_10261, gj_wifi_ok_u_10262, gj_ethernet_ok_u_10263,
 * gj_dns_ok_u_10264, gj_https_ok_u_10265, gj_cdn_ok_u_10266,
 * gj_offline_mode_ok_u_10268, gj_net_soft_ready_u_10269, and
 * gj_batch_id_10270. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10267_marker[] = "libcgj-batch10267";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B10267_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10267_steam_net_ok(void)
{
	return B10267_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_10267 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_10267(void)
{
	(void)NULL;
	return b10267_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_10267(void)
    __attribute__((alias("gj_steam_net_ok_u_10267")));
