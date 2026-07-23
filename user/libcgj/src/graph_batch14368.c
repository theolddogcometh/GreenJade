/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14368: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_14368(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode capability readiness;
 *       not a runtime offline-mode capability probe.
 *   uint32_t __gj_offline_mode_ok_u_14368  (alias)
 *   __libcgj_batch14368_marker = "libcgj-batch14368"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_offline_mode_ok_u_14368 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_13868 (batch13868), gj_offline_mode_ok_u_13668
 * (batch13668), gj_offline_mode_ok_u_13468 (batch13468),
 * gj_offline_mode_ok_u_13268 (batch13268), gj_net_ok_u_14361,
 * gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363, gj_dns_ok_u_14364,
 * gj_https_ok_u_14365, gj_cdn_ok_u_14366, gj_steam_net_ok_u_14367,
 * gj_net_soft_ready_u_14369, and gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14368_marker[] = "libcgj-batch14368";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B14368_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14368_offline_mode_ok(void)
{
	return B14368_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_14368 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_14368(void)
{
	(void)NULL;
	return b14368_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_14368(void)
    __attribute__((alias("gj_offline_mode_ok_u_14368")));
