/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11964: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_11964(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolver readiness; not a runtime probe of
 *       resolv.conf or name servers.
 *   uint32_t __gj_dns_ok_u_11964  (alias)
 *   __libcgj_batch11964_marker = "libcgj-batch11964"
 *
 * Net/connectivity soft product CREATE-ONLY (11961-11970). Unique
 * gj_dns_ok_u_11964 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_11764 (batch11764), gj_dns_ok_u_11564 (batch11564),
 * gj_net_ok_u_11961, gj_wifi_ok_u_11962, gj_ethernet_ok_u_11963,
 * gj_https_ok_u_11965, gj_cdn_ok_u_11966, gj_steam_net_ok_u_11967,
 * gj_offline_mode_ok_u_11968, gj_net_soft_ready_u_11969, and
 * gj_batch_id_11970. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11964_marker[] = "libcgj-batch11964";

/* Soft dns-ok product lamp (unset / not probed). */
#define B11964_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11964_dns_ok(void)
{
	return B11964_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_11964 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * name resolution. No parent wires.
 */
uint32_t
gj_dns_ok_u_11964(void)
{
	(void)NULL;
	return b11964_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_11964(void)
    __attribute__((alias("gj_dns_ok_u_11964")));
