/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12861: net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_12861(void);
 *     - Returns 0 (soft net-ok product lamp unset). Soft compile-time
 *       product tag for generic network connectivity readiness; not a
 *       runtime probe of the network stack.
 *   uint32_t __gj_net_ok_u_12861  (alias)
 *   __libcgj_batch12861_marker = "libcgj-batch12861"
 *
 * Net/connectivity soft product CREATE-ONLY (12861-12870). Unique
 * gj_net_ok_u_12861 surface only; no multi-def. Distinct from
 * gj_net_ok_u_12661 (batch12661), gj_net_ok_u_12461 (batch12461),
 * gj_net_ok_u_12261 (batch12261), gj_wifi_ok_u_12862,
 * gj_ethernet_ok_u_12863, gj_dns_ok_u_12864, gj_https_ok_u_12865,
 * gj_cdn_ok_u_12866, gj_steam_net_ok_u_12867,
 * gj_offline_mode_ok_u_12868, gj_net_soft_ready_u_12869, and
 * gj_batch_id_12870. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12861_marker[] = "libcgj-batch12861";

/* Soft net-ok product lamp (unset / not probed). */
#define B12861_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12861_net_ok(void)
{
	return B12861_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_12861 - report soft generic net connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the net soft surface. Does not call libc or probe
 * the network. No parent wires.
 */
uint32_t
gj_net_ok_u_12861(void)
{
	(void)NULL;
	return b12861_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_12861(void)
    __attribute__((alias("gj_net_ok_u_12861")));
