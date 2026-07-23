/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10065: network product soft ntp-ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ntp_ok_u_10065(void);
 *     - Returns 0 (ntp soft path not asserted for the network product
 *       soft-id wave). Soft compile-time product tag; not a live
 *       clock-sync or peer probe.
 *   uint32_t __gj_ntp_ok_u_10065  (alias)
 *   __libcgj_batch10065_marker = "libcgj-batch10065"
 *
 * Exclusive network product soft id CREATE-ONLY (10061-10070:
 * wifi_ok, ethernet_ok, dns_ok, dhcp_ok, ntp_ok, https_ok, cdn_ok,
 * steam_cdn_ok, net_soft_ready, batch_id_10070). Unique
 * gj_ntp_ok_u_10065 surface only; no multi-def. Distinct from
 * gj_ntp_parse / gj_ntp32_to_unix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10065_marker[] = "libcgj-batch10065";

/* Network product soft ntp-ok lamp (not asserted). */
#define B10065_NTP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10065_ok(void)
{
	return B10065_NTP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ntp_ok_u_10065 - network product soft ntp-ok lamp.
 *
 * Always returns 0. Soft pure-data product tag. Does not sync
 * clocks or call libc. No parent wires.
 */
uint32_t
gj_ntp_ok_u_10065(void)
{
	(void)NULL;
	return b10065_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ntp_ok_u_10065(void)
    __attribute__((alias("gj_ntp_ok_u_10065")));
