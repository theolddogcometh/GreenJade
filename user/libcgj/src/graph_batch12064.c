/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12064: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_12064(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolver readiness; not a runtime probe of
 *       resolv.conf or name servers.
 *   uint32_t __gj_dns_ok_u_12064  (alias)
 *   __libcgj_batch12064_marker = "libcgj-batch12064"
 *
 * Net/connectivity soft product CREATE-ONLY (12061-12070). Unique
 * gj_dns_ok_u_12064 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_11964 (batch11964), gj_dns_ok_u_11764 (batch11764),
 * gj_net_ok_u_12061, gj_wifi_ok_u_12062, gj_ethernet_ok_u_12063,
 * gj_https_ok_u_12065, gj_cdn_ok_u_12066, gj_steam_net_ok_u_12067,
 * gj_offline_mode_ok_u_12068, gj_net_soft_ready_u_12069, and
 * gj_batch_id_12070. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12064_marker[] = "libcgj-batch12064";

/* Soft dns-ok product lamp (unset / not probed). */
#define B12064_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12064_dns_ok(void)
{
	return B12064_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_12064 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * name resolution. No parent wires.
 */
uint32_t
gj_dns_ok_u_12064(void)
{
	(void)NULL;
	return b12064_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_12064(void)
    __attribute__((alias("gj_dns_ok_u_12064")));
