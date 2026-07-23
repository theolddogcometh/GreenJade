/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13864: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_13864(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolution readiness; not a runtime probe
 *       of resolvers or name servers.
 *   uint32_t __gj_dns_ok_u_13864  (alias)
 *   __libcgj_batch13864_marker = "libcgj-batch13864"
 *
 * Net/connectivity soft product CREATE-ONLY (13861-13870). Unique
 * gj_dns_ok_u_13864 surface only; no multi-def. Distinct from
 * gj_net_ok_u_13861, gj_wifi_ok_u_13862, gj_ethernet_ok_u_13863,
 * gj_https_ok_u_13865, gj_cdn_ok_u_13866, gj_steam_net_ok_u_13867,
 * gj_offline_mode_ok_u_13868, gj_net_soft_ready_u_13869, and
 * gj_batch_id_13870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13864_marker[] = "libcgj-batch13864";

/* Soft dns-ok product lamp (unset / not probed). */
#define B13864_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13864_dns_ok(void)
{
	return B13864_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_13864 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * DNS. No parent wires.
 */
uint32_t
gj_dns_ok_u_13864(void)
{
	(void)NULL;
	return b13864_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_13864(void)
    __attribute__((alias("gj_dns_ok_u_13864")));
