/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11968: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_11968(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode capability readiness;
 *       not a runtime offline-mode capability probe.
 *   uint32_t __gj_offline_mode_ok_u_11968  (alias)
 *   __libcgj_batch11968_marker = "libcgj-batch11968"
 *
 * Net/connectivity soft product CREATE-ONLY (11961-11970). Unique
 * gj_offline_mode_ok_u_11968 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_11768 (batch11768), gj_offline_mode_ok_u_11568
 * (batch11568), gj_net_ok_u_11961, gj_wifi_ok_u_11962,
 * gj_ethernet_ok_u_11963, gj_dns_ok_u_11964, gj_https_ok_u_11965,
 * gj_cdn_ok_u_11966, gj_steam_net_ok_u_11967,
 * gj_net_soft_ready_u_11969, and gj_batch_id_11970. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11968_marker[] = "libcgj-batch11968";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B11968_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11968_offline_mode_ok(void)
{
	return B11968_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_11968 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_11968(void)
{
	(void)NULL;
	return b11968_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_11968(void)
    __attribute__((alias("gj_offline_mode_ok_u_11968")));
