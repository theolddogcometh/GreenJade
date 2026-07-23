/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12663: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_12663(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired ethernet connectivity
 *       readiness; not a runtime probe of NIC hardware or drivers.
 *   uint32_t __gj_ethernet_ok_u_12663  (alias)
 *   __libcgj_batch12663_marker = "libcgj-batch12663"
 *
 * Net/connectivity soft product CREATE-ONLY (12661-12670). Unique
 * gj_ethernet_ok_u_12663 surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_12463 (batch12463), gj_ethernet_ok_u_12263
 * (batch12263), gj_net_ok_u_12661, gj_wifi_ok_u_12662,
 * gj_dns_ok_u_12664, gj_https_ok_u_12665, gj_cdn_ok_u_12666,
 * gj_steam_net_ok_u_12667, gj_offline_mode_ok_u_12668,
 * gj_net_soft_ready_u_12669, and gj_batch_id_12670. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12663_marker[] = "libcgj-batch12663";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B12663_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12663_ethernet_ok(void)
{
	return B12663_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_12663 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe NICs. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_12663(void)
{
	(void)NULL;
	return b12663_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_12663(void)
    __attribute__((alias("gj_ethernet_ok_u_12663")));
