/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11564: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_11564(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolver readiness; not a runtime probe of
 *       name resolution or resolv.conf.
 *   uint32_t __gj_dns_ok_u_11564  (alias)
 *   __libcgj_batch11564_marker = "libcgj-batch11564"
 *
 * Net/connectivity soft product CREATE-ONLY (11561-11570). Unique
 * gj_dns_ok_u_11564 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_11364 (batch11364), gj_dns_ok_u_11164 (batch11164),
 * gj_net_ok_u_11561, gj_wifi_ok_u_11562, gj_ethernet_ok_u_11563,
 * gj_https_ok_u_11565, gj_cdn_ok_u_11566, gj_steam_net_ok_u_11567,
 * gj_offline_mode_ok_u_11568, gj_net_soft_ready_u_11569, and
 * gj_batch_id_11570. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11564_marker[] = "libcgj-batch11564";

/* Soft dns-ok product lamp (unset / not probed). */
#define B11564_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11564_dns_ok(void)
{
	return B11564_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_11564 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * DNS. No parent wires.
 */
uint32_t
gj_dns_ok_u_11564(void)
{
	(void)NULL;
	return b11564_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_11564(void)
    __attribute__((alias("gj_dns_ok_u_11564")));
