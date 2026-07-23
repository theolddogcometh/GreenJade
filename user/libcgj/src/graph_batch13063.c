/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13063: ethernet connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_13063(void);
 *     - Returns 0 (soft ethernet-ok product lamp unset). Soft
 *       compile-time product tag for wired ethernet connectivity
 *       readiness; not a runtime probe of NIC hardware or drivers.
 *   uint32_t __gj_ethernet_ok_u_13063  (alias)
 *   __libcgj_batch13063_marker = "libcgj-batch13063"
 *
 * Net/connectivity soft product CREATE-ONLY (13061-13070). Unique
 * gj_ethernet_ok_u_13063 surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_12863 (batch12863), gj_ethernet_ok_u_12663
 * (batch12663), gj_ethernet_ok_u_12463 (batch12463),
 * gj_ethernet_ok_u_12263 (batch12263), gj_net_ok_u_13061,
 * gj_wifi_ok_u_13062, gj_dns_ok_u_13064, gj_https_ok_u_13065,
 * gj_cdn_ok_u_13066, gj_steam_net_ok_u_13067,
 * gj_offline_mode_ok_u_13068, gj_net_soft_ready_u_13069, and
 * gj_batch_id_13070. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13063_marker[] = "libcgj-batch13063";

/* Soft ethernet-ok product lamp (unset / not probed). */
#define B13063_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13063_ethernet_ok(void)
{
	return B13063_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_13063 - report soft ethernet connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the ethernet soft surface. Does not call libc or
 * probe NICs. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_13063(void)
{
	(void)NULL;
	return b13063_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_13063(void)
    __attribute__((alias("gj_ethernet_ok_u_13063")));
