/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11363: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_11363(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired LAN connectivity readiness;
 *       not a runtime probe of ethernet hardware or drivers.
 *   uint32_t __gj_ethernet_ok_u_11363  (alias)
 *   __libcgj_batch11363_marker = "libcgj-batch11363"
 *
 * Net/connectivity soft product CREATE-ONLY (11361-11370). Unique
 * gj_ethernet_ok_u_11363 surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_11163 (batch11163), gj_ethernet_ok_u_10963
 * (batch10963), gj_net_ok_u_11361, gj_wifi_ok_u_11362,
 * gj_dns_ok_u_11364, gj_https_ok_u_11365, gj_cdn_ok_u_11366,
 * gj_steam_net_ok_u_11367, gj_offline_mode_ok_u_11368,
 * gj_net_soft_ready_u_11369, and gj_batch_id_11370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11363_marker[] = "libcgj-batch11363";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B11363_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11363_ethernet_ok(void)
{
	return B11363_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_11363 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe ethernet. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_11363(void)
{
	(void)NULL;
	return b11363_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_11363(void)
    __attribute__((alias("gj_ethernet_ok_u_11363")));
