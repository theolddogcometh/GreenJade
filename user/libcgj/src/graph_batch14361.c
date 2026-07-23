/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14361: net connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_14361(void);
 *     - Returns 0 (soft net-ok product lamp unset). Soft compile-time
 *       product tag for generic network connectivity readiness; not a
 *       runtime probe of the network stack.
 *   uint32_t __gj_net_ok_u_14361  (alias)
 *   __libcgj_batch14361_marker = "libcgj-batch14361"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_net_ok_u_14361 surface only; no multi-def. Distinct from
 * gj_net_ok_u_13861 (batch13861), gj_net_ok_u_13661 (batch13661),
 * gj_net_ok_u_13461 (batch13461), gj_net_ok_u_13261 (batch13261),
 * gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363, gj_dns_ok_u_14364,
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

const char __libcgj_batch14361_marker[] = "libcgj-batch14361";

/* Soft net-ok product lamp (unset / not probed). */
#define B14361_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14361_net_ok(void)
{
	return B14361_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_14361 - report soft generic net connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the net soft surface. Does not call libc or probe
 * the network. No parent wires.
 */
uint32_t
gj_net_ok_u_14361(void)
{
	(void)NULL;
	return b14361_net_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_14361(void)
    __attribute__((alias("gj_net_ok_u_14361")));
