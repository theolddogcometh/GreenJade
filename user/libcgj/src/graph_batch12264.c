/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12264: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_12264(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolution readiness; not a runtime probe
 *       of resolvers or nameservers.
 *   uint32_t __gj_dns_ok_u_12264  (alias)
 *   __libcgj_batch12264_marker = "libcgj-batch12264"
 *
 * Net/connectivity soft product CREATE-ONLY (12261-12270). Unique
 * gj_dns_ok_u_12264 surface only; no multi-def. Distinct from
 * gj_dns_ok_u_12064 (batch12064), gj_dns_ok_u_11964 (batch11964),
 * gj_net_ok_u_12261, gj_wifi_ok_u_12262, gj_ethernet_ok_u_12263,
 * gj_https_ok_u_12265, gj_cdn_ok_u_12266, gj_steam_net_ok_u_12267,
 * gj_offline_mode_ok_u_12268, gj_net_soft_ready_u_12269, and
 * gj_batch_id_12270. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12264_marker[] = "libcgj-batch12264";

/* Soft dns-ok product lamp (unset / not probed). */
#define B12264_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12264_dns_ok(void)
{
	return B12264_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_12264 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * resolvers. No parent wires.
 */
uint32_t
gj_dns_ok_u_12264(void)
{
	(void)NULL;
	return b12264_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_12264(void)
    __attribute__((alias("gj_dns_ok_u_12264")));
