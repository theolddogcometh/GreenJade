/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11168: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_11168(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline/local-only mode readiness;
 *       not a runtime probe of network or package state.
 *   uint32_t __gj_offline_mode_ok_u_11168  (alias)
 *   __libcgj_batch11168_marker = "libcgj-batch11168"
 *
 * Net/connectivity soft product CREATE-ONLY (11161-11170). Unique
 * gj_offline_mode_ok_u_11168 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_10968 (batch10968), gj_net_ok_u_11161,
 * gj_wifi_ok_u_11162, gj_ethernet_ok_u_11163, gj_dns_ok_u_11164,
 * gj_https_ok_u_11165, gj_cdn_ok_u_11166, gj_steam_net_ok_u_11167,
 * gj_net_soft_ready_u_11169, and gj_batch_id_11170. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11168_marker[] = "libcgj-batch11168";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B11168_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11168_offline_mode_ok(void)
{
	return B11168_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_11168 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc
 * or probe network state. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_11168(void)
{
	(void)NULL;
	return b11168_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_11168(void)
    __attribute__((alias("gj_offline_mode_ok_u_11168")));
