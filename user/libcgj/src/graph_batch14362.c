/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14362: wifi connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wifi_ok_u_14362(void);
 *     - Returns 0 (soft wifi-ok product lamp unset). Soft compile-time
 *       product tag for wireless LAN connectivity readiness; not a
 *       runtime probe of wifi hardware or drivers.
 *   uint32_t __gj_wifi_ok_u_14362  (alias)
 *   __libcgj_batch14362_marker = "libcgj-batch14362"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_wifi_ok_u_14362 surface only; no multi-def. Distinct from
 * gj_wifi_ok_u_13862 (batch13862), gj_wifi_ok_u_13662 (batch13662),
 * gj_wifi_ok_u_13462 (batch13462), gj_wifi_ok_u_13262 (batch13262),
 * gj_net_ok_u_14361, gj_ethernet_ok_u_14363, gj_dns_ok_u_14364,
 * gj_https_ok_u_14365, gj_cdn_ok_u_14366, gj_steam_net_ok_u_14367,
 * gj_offline_mode_ok_u_14368, gj_net_soft_ready_u_14369, and
 * gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14362_marker[] = "libcgj-batch14362";

/* Soft wifi-ok product lamp (unset / not probed). */
#define B14362_WIFI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14362_wifi_ok(void)
{
	return B14362_WIFI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_ok_u_14362 - report soft wifi connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the wifi soft surface. Does not call libc or probe
 * wifi. No parent wires.
 */
uint32_t
gj_wifi_ok_u_14362(void)
{
	(void)NULL;
	return b14362_wifi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wifi_ok_u_14362(void)
    __attribute__((alias("gj_wifi_ok_u_14362")));
