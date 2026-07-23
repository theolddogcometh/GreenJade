/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12461: net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_12461(void);
 *     - Returns 0 (soft net-ok product lamp unset). Soft compile-time
 *       product tag for generic network connectivity readiness; not a
 *       runtime probe of the network stack.
 *   uint32_t __gj_net_ok_u_12461  (alias)
 *   __libcgj_batch12461_marker = "libcgj-batch12461"
 *
 * Net/connectivity soft product CREATE-ONLY (12461-12470). Unique
 * gj_net_ok_u_12461 surface only; no multi-def. Distinct from
 * gj_net_ok_u_12061 (batch12061), gj_net_ok_u_11961 (batch11961),
 * gj_net_ok_u_11761 (batch11761), gj_wifi_ok_u_12462,
 * gj_ethernet_ok_u_12463, gj_dns_ok_u_12464, gj_https_ok_u_12465,
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

const char __libcgj_batch12461_marker[] = "libcgj-batch12461";

/* Soft net-ok product lamp (unset / not probed). */
#define B12461_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12461_net_ok(void)
{
	return B12461_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_12461 - report soft generic net connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the net soft surface. Does not call libc or probe
 * the network. No parent wires.
 */
uint32_t
gj_net_ok_u_12461(void)
{
	(void)NULL;
	return b12461_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_12461(void)
    __attribute__((alias("gj_net_ok_u_12461")));
