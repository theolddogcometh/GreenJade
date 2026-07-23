/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13863: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_13863(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired LAN connectivity readiness;
 *       not a runtime probe of ethernet hardware or drivers.
 *   uint32_t __gj_ethernet_ok_u_13863  (alias)
 *   __libcgj_batch13863_marker = "libcgj-batch13863"
 *
 * Net/connectivity soft product CREATE-ONLY (13861-13870). Unique
 * gj_ethernet_ok_u_13863 surface only; no multi-def. Distinct from
 * gj_net_ok_u_13861, gj_wifi_ok_u_13862, gj_dns_ok_u_13864,
 * gj_https_ok_u_13865, gj_cdn_ok_u_13866, gj_steam_net_ok_u_13867,
 * gj_offline_mode_ok_u_13868, gj_net_soft_ready_u_13869, and
 * gj_batch_id_13870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13863_marker[] = "libcgj-batch13863";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B13863_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13863_ethernet_ok(void)
{
	return B13863_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_13863 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe ethernet. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_13863(void)
{
	(void)NULL;
	return b13863_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_13863(void)
    __attribute__((alias("gj_ethernet_ok_u_13863")));
