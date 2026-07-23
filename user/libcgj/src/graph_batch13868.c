/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13868: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_13868(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline-mode path readiness; not
 *       a runtime probe of connectivity or cache state.
 *   uint32_t __gj_offline_mode_ok_u_13868  (alias)
 *   __libcgj_batch13868_marker = "libcgj-batch13868"
 *
 * Net/connectivity soft product CREATE-ONLY (13861-13870). Unique
 * gj_offline_mode_ok_u_13868 surface only; no multi-def. Distinct from
 * gj_net_ok_u_13861, gj_wifi_ok_u_13862, gj_ethernet_ok_u_13863,
 * gj_dns_ok_u_13864, gj_https_ok_u_13865, gj_cdn_ok_u_13866,
 * gj_steam_net_ok_u_13867, gj_net_soft_ready_u_13869, and
 * gj_batch_id_13870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13868_marker[] = "libcgj-batch13868";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B13868_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13868_offline_mode_ok(void)
{
	return B13868_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_13868 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc
 * or probe connectivity. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_13868(void)
{
	(void)NULL;
	return b13868_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_13868(void)
    __attribute__((alias("gj_offline_mode_ok_u_13868")));
