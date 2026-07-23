/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12263: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_12263(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired Ethernet readiness; not a
 *       runtime probe of eth interfaces or drivers.
 *   uint32_t __gj_ethernet_ok_u_12263  (alias)
 *   __libcgj_batch12263_marker = "libcgj-batch12263"
 *
 * Net/connectivity soft product CREATE-ONLY (12261-12270). Unique
 * gj_ethernet_ok_u_12263 surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_12063 (batch12063), gj_ethernet_ok_u_11963
 * (batch11963), gj_net_ok_u_12261, gj_wifi_ok_u_12262,
 * gj_dns_ok_u_12264, gj_https_ok_u_12265, gj_cdn_ok_u_12266,
 * gj_steam_net_ok_u_12267, gj_offline_mode_ok_u_12268,
 * gj_net_soft_ready_u_12269, and gj_batch_id_12270. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12263_marker[] = "libcgj-batch12263";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B12263_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12263_ethernet_ok(void)
{
	return B12263_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_12263 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe interfaces. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_12263(void)
{
	(void)NULL;
	return b12263_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_12263(void)
    __attribute__((alias("gj_ethernet_ok_u_12263")));
