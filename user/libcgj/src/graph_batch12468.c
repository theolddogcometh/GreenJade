/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12468: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_12468(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode capability readiness;
 *       not a runtime offline-mode capability probe.
 *   uint32_t __gj_offline_mode_ok_u_12468  (alias)
 *   __libcgj_batch12468_marker = "libcgj-batch12468"
 *
 * Net/connectivity soft product CREATE-ONLY (12461-12470). Unique
 * gj_offline_mode_ok_u_12468 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_12068 (batch12068), gj_offline_mode_ok_u_11968
 * (batch11968), gj_offline_mode_ok_u_11768 (batch11768),
 * gj_net_ok_u_12461, gj_wifi_ok_u_12462, gj_ethernet_ok_u_12463,
 * gj_dns_ok_u_12464, gj_https_ok_u_12465, gj_cdn_ok_u_12466,
 * gj_steam_net_ok_u_12467, gj_net_soft_ready_u_12469, and
 * gj_batch_id_12470. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12468_marker[] = "libcgj-batch12468";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B12468_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12468_offline_mode_ok(void)
{
	return B12468_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_12468 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_12468(void)
{
	(void)NULL;
	return b12468_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_12468(void)
    __attribute__((alias("gj_offline_mode_ok_u_12468")));
