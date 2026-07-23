/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12268: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_12268(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode capability readiness;
 *       not a runtime offline-mode capability probe.
 *   uint32_t __gj_offline_mode_ok_u_12268  (alias)
 *   __libcgj_batch12268_marker = "libcgj-batch12268"
 *
 * Net/connectivity soft product CREATE-ONLY (12261-12270). Unique
 * gj_offline_mode_ok_u_12268 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_12068 (batch12068), gj_offline_mode_ok_u_11968
 * (batch11968), gj_net_ok_u_12261, gj_wifi_ok_u_12262,
 * gj_ethernet_ok_u_12263, gj_dns_ok_u_12264, gj_https_ok_u_12265,
 * gj_cdn_ok_u_12266, gj_steam_net_ok_u_12267,
 * gj_net_soft_ready_u_12269, and gj_batch_id_12270. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12268_marker[] = "libcgj-batch12268";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B12268_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12268_offline_mode_ok(void)
{
	return B12268_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_12268 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_12268(void)
{
	(void)NULL;
	return b12268_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_12268(void)
    __attribute__((alias("gj_offline_mode_ok_u_12268")));
