/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14367: Steam net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_14367(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness;
 *       not a runtime probe of Steam client or CDN.
 *   uint32_t __gj_steam_net_ok_u_14367  (alias)
 *   __libcgj_batch14367_marker = "libcgj-batch14367"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_steam_net_ok_u_14367 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_13867 (batch13867), gj_steam_net_ok_u_13667
 * (batch13667), gj_steam_net_ok_u_13467 (batch13467),
 * gj_steam_net_ok_u_13267 (batch13267), gj_net_ok_u_14361,
 * gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363, gj_dns_ok_u_14364,
 * gj_https_ok_u_14365, gj_cdn_ok_u_14366,
 * gj_offline_mode_ok_u_14368, gj_net_soft_ready_u_14369, and
 * gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14367_marker[] = "libcgj-batch14367";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B14367_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14367_steam_net_ok(void)
{
	return B14367_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_14367 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft surface. Does not call libc or
 * probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_14367(void)
{
	(void)NULL;
	return b14367_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_14367(void)
    __attribute__((alias("gj_steam_net_ok_u_14367")));
