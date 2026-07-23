/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12262: wifi connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wifi_ok_u_12262(void);
 *     - Returns 0 (soft wifi-ok product lamp unset). Soft compile-time
 *       product tag for wireless LAN connectivity readiness; not a
 *       runtime probe of wifi hardware or drivers.
 *   uint32_t __gj_wifi_ok_u_12262  (alias)
 *   __libcgj_batch12262_marker = "libcgj-batch12262"
 *
 * Net/connectivity soft product CREATE-ONLY (12261-12270). Unique
 * gj_wifi_ok_u_12262 surface only; no multi-def. Distinct from
 * gj_wifi_ok_u_12062 (batch12062), gj_wifi_ok_u_11962 (batch11962),
 * gj_net_ok_u_12261, gj_ethernet_ok_u_12263, gj_dns_ok_u_12264,
 * gj_https_ok_u_12265, gj_cdn_ok_u_12266, gj_steam_net_ok_u_12267,
 * gj_offline_mode_ok_u_12268, gj_net_soft_ready_u_12269, and
 * gj_batch_id_12270. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12262_marker[] = "libcgj-batch12262";

/* Soft wifi-ok product lamp (unset / not probed). */
#define B12262_WIFI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12262_wifi_ok(void)
{
	return B12262_WIFI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_ok_u_12262 - report soft wifi connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the wifi soft surface. Does not call libc or probe
 * wifi. No parent wires.
 */
uint32_t
gj_wifi_ok_u_12262(void)
{
	(void)NULL;
	return b12262_wifi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wifi_ok_u_12262(void)
    __attribute__((alias("gj_wifi_ok_u_12262")));
