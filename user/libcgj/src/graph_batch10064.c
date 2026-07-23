/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10064: network product soft dhcp-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dhcp_ok_u_10064(void);
 *     - Returns 0 (dhcp soft path not asserted for the network product
 *       soft-id wave). Soft compile-time product tag; not a live
 *       lease or DHCP client probe.
 *   uint32_t __gj_dhcp_ok_u_10064  (alias)
 *   __libcgj_batch10064_marker = "libcgj-batch10064"
 *
 * Exclusive network product soft id CREATE-ONLY (10061-10070:
 * wifi_ok, ethernet_ok, dns_ok, dhcp_ok, ntp_ok, https_ok, cdn_ok,
 * steam_cdn_ok, net_soft_ready, batch_id_10070). Unique
 * gj_dhcp_ok_u_10064 surface only; no multi-def. Distinct from
 * gj_dhcp_find_option / gj_dhcp_check_magic. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10064_marker[] = "libcgj-batch10064";

/* Network product soft dhcp-ok lamp (not asserted). */
#define B10064_DHCP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10064_ok(void)
{
	return B10064_DHCP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dhcp_ok_u_10064 - network product soft dhcp-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not probe
 * leases or call libc. No parent wires.
 */
uint32_t
gj_dhcp_ok_u_10064(void)
{
	(void)NULL;
	return b10064_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dhcp_ok_u_10064(void)
    __attribute__((alias("gj_dhcp_ok_u_10064")));
