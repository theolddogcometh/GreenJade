/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10964: DNS connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_10964(void);
 *     - Returns 0 (soft dns-ok product lamp unset). Soft compile-time
 *       product tag for DNS resolution readiness; not a runtime probe
 *       of resolvers or nameservers.
 *   uint32_t __gj_dns_ok_u_10964  (alias)
 *   __libcgj_batch10964_marker = "libcgj-batch10964"
 *
 * Net/connectivity soft product CREATE-ONLY (10961-10970). Unique
 * gj_dns_ok_u_10964 surface only; no multi-def. Distinct from
 * gj_net_ok_u_10961, gj_wifi_ok_u_10962, gj_ethernet_ok_u_10963,
 * gj_https_ok_u_10965, gj_cdn_ok_u_10966, gj_steam_net_ok_u_10967,
 * gj_offline_mode_ok_u_10968, gj_net_soft_ready_u_10969, and
 * gj_batch_id_10970. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10964_marker[] = "libcgj-batch10964";

/* Soft dns-ok product lamp (unset / not probed). */
#define B10964_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10964_dns_ok(void)
{
	return B10964_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_10964 - report soft DNS connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the DNS soft surface. Does not call libc or probe
 * resolvers. No parent wires.
 */
uint32_t
gj_dns_ok_u_10964(void)
{
	(void)NULL;
	return b10964_dns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_10964(void)
    __attribute__((alias("gj_dns_ok_u_10964")));
