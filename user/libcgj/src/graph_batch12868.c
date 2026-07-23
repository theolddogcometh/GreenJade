/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12868: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_12868(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode capability readiness;
 *       not a runtime offline-mode capability probe.
 *   uint32_t __gj_offline_mode_ok_u_12868  (alias)
 *   __libcgj_batch12868_marker = "libcgj-batch12868"
 *
 * Net/connectivity soft product CREATE-ONLY (12861-12870). Unique
 * gj_offline_mode_ok_u_12868 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_12668 (batch12668), gj_offline_mode_ok_u_12468
 * (batch12468), gj_offline_mode_ok_u_12268 (batch12268),
 * gj_net_ok_u_12861, gj_wifi_ok_u_12862, gj_ethernet_ok_u_12863,
 * gj_dns_ok_u_12864, gj_https_ok_u_12865, gj_cdn_ok_u_12866,
 * gj_steam_net_ok_u_12867, gj_net_soft_ready_u_12869, and
 * gj_batch_id_12870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12868_marker[] = "libcgj-batch12868";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B12868_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12868_offline_mode_ok(void)
{
	return B12868_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_12868 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_12868(void)
{
	(void)NULL;
	return b12868_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_12868(void)
    __attribute__((alias("gj_offline_mode_ok_u_12868")));
