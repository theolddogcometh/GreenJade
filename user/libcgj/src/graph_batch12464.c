/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12464: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_12464(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolver readiness; not a runtime probe of
 *       resolv.conf, systemd-resolved, or name servers.
 *   uint32_t __gj_dns_ok_u_12464  (alias)
 *   __libcgj_batch12464_marker = "libcgj-batch12464"
 *
 * Net/connectivity soft product CREATE-ONLY (12461-12470). Unique
 * gj_dns_ok_u_12464 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_12064 (batch12064), gj_dns_ok_u_11964 (batch11964),
 * gj_dns_ok_u_11764 (batch11764), gj_net_ok_u_12461,
 * gj_wifi_ok_u_12462, gj_ethernet_ok_u_12463, gj_https_ok_u_12465,
 * gj_cdn_ok_u_12466, gj_steam_net_ok_u_12467,
 * gj_offline_mode_ok_u_12468, gj_net_soft_ready_u_12469, and
 * gj_batch_id_12470. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12464_marker[] = "libcgj-batch12464";

/* Soft dns-ok product lamp (unset / not probed). */
#define B12464_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12464_dns_ok(void)
{
	return B12464_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_12464 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * resolvers. No parent wires.
 */
uint32_t
gj_dns_ok_u_12464(void)
{
	(void)NULL;
	return b12464_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_12464(void)
    __attribute__((alias("gj_dns_ok_u_12464")));
