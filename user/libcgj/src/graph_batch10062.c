/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10062: network product soft ethernet-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_10062(void);
 *     - Returns 0 (ethernet soft path not asserted for the network
 *       product soft-id wave). Soft compile-time product tag; not a
 *       live link or carrier probe.
 *   uint32_t __gj_ethernet_ok_u_10062  (alias)
 *   __libcgj_batch10062_marker = "libcgj-batch10062"
 *
 * Exclusive network product soft id CREATE-ONLY (10061-10070:
 * wifi_ok, ethernet_ok, dns_ok, dhcp_ok, ntp_ok, https_ok, cdn_ok,
 * steam_cdn_ok, net_soft_ready, batch_id_10070). Unique
 * gj_ethernet_ok_u_10062 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10062_marker[] = "libcgj-batch10062";

/* Network product soft ethernet-ok lamp (not asserted). */
#define B10062_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10062_ok(void)
{
	return B10062_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_10062 - network product soft ethernet-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not probe
 * links or call libc. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_10062(void)
{
	(void)NULL;
	return b10062_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_10062(void)
    __attribute__((alias("gj_ethernet_ok_u_10062")));
