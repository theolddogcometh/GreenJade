/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14364: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_14364(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolver readiness; not a runtime probe of
 *       resolv.conf or name servers.
 *   uint32_t __gj_dns_ok_u_14364  (alias)
 *   __libcgj_batch14364_marker = "libcgj-batch14364"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_dns_ok_u_14364 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_13864 (batch13864), gj_dns_ok_u_13664 (batch13664),
 * gj_dns_ok_u_13464 (batch13464), gj_dns_ok_u_13264 (batch13264),
 * gj_net_ok_u_14361, gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363,
 * gj_https_ok_u_14365, gj_cdn_ok_u_14366, gj_steam_net_ok_u_14367,
 * gj_offline_mode_ok_u_14368, gj_net_soft_ready_u_14369, and
 * gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14364_marker[] = "libcgj-batch14364";

/* Soft dns-ok product lamp (unset / not probed). */
#define B14364_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14364_dns_ok(void)
{
	return B14364_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_14364 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * name resolution. No parent wires.
 */
uint32_t
gj_dns_ok_u_14364(void)
{
	(void)NULL;
	return b14364_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_14364(void)
    __attribute__((alias("gj_dns_ok_u_14364")));
