/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14363: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_14363(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired ethernet connectivity
 *       readiness; not a runtime probe of NIC hardware or drivers.
 *   uint32_t __gj_ethernet_ok_u_14363  (alias)
 *   __libcgj_batch14363_marker = "libcgj-batch14363"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_ethernet_ok_u_14363 surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_13863 (batch13863), gj_ethernet_ok_u_13663
 * (batch13663), gj_ethernet_ok_u_13463 (batch13463),
 * gj_ethernet_ok_u_13263 (batch13263), gj_net_ok_u_14361,
 * gj_wifi_ok_u_14362, gj_dns_ok_u_14364, gj_https_ok_u_14365,
 * gj_cdn_ok_u_14366, gj_steam_net_ok_u_14367,
 * gj_offline_mode_ok_u_14368, gj_net_soft_ready_u_14369, and
 * gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14363_marker[] = "libcgj-batch14363";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B14363_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14363_ethernet_ok(void)
{
	return B14363_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_14363 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe NICs. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_14363(void)
{
	(void)NULL;
	return b14363_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_14363(void)
    __attribute__((alias("gj_ethernet_ok_u_14363")));
