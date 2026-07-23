/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14167: Steam networking soft deepen product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_net_ok_u_14167(void);
 *     - Returns 0 (soft steam-net-ok product lamp unset). Soft
 *       compile-time product tag for Steam networking path readiness
 *       deepen; not a runtime probe of Steam services or clients.
 *   uint32_t __gj_steam_net_ok_u_14167  (alias)
 *   __libcgj_batch14167_marker = "libcgj-batch14167"
 *
 * Exclusive continuum CREATE-ONLY net soft deepen (14161-14170). Unique
 * gj_steam_net_ok_u_14167 surface only; no multi-def. Distinct from
 * gj_steam_net_ok_u_13867 (batch13867), gj_net_ok_u_14161,
 * gj_wifi_ok_u_14162, gj_ethernet_ok_u_14163, gj_dns_ok_u_14164,
 * gj_https_ok_u_14165, gj_cdn_ok_u_14166, gj_offline_mode_ok_u_14168,
 * gj_net_soft_ready_u_14169, and gj_batch_id_14170. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14167_marker[] = "libcgj-batch14167";

/* Soft steam-net-ok product lamp (unset / not probed). */
#define B14167_STEAM_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14167_steam_net_ok(void)
{
	return B14167_STEAM_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_net_ok_u_14167 - report soft Steam networking ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the Steam-net soft deepen surface. Does not call
 * libc or probe Steam. No parent wires.
 */
uint32_t
gj_steam_net_ok_u_14167(void)
{
	(void)NULL;
	return b14167_steam_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_net_ok_u_14167(void)
    __attribute__((alias("gj_steam_net_ok_u_14167")));
