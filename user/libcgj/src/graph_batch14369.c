/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14369: net soft-ready aggregate product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_soft_ready_u_14369(void);
 *     - Returns 0 (soft net soft-ready product lamp unset). Soft
 *       compile-time aggregate product tag for the net soft wave; not a
 *       runtime probe of any network path.
 *   uint32_t __gj_net_soft_ready_u_14369  (alias)
 *   __libcgj_batch14369_marker = "libcgj-batch14369"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_net_soft_ready_u_14369 surface only; no multi-def. Distinct from
 * gj_net_soft_ready_u_13869 (batch13869), gj_net_soft_ready_u_13669
 * (batch13669), gj_net_soft_ready_u_13469 (batch13469),
 * gj_net_soft_ready_u_13269 (batch13269), gj_net_ok_u_14361,
 * gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363, gj_dns_ok_u_14364,
 * gj_https_ok_u_14365, gj_cdn_ok_u_14366, gj_steam_net_ok_u_14367,
 * gj_offline_mode_ok_u_14368, and gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14369_marker[] = "libcgj-batch14369";

/* Soft net soft-ready aggregate product lamp (unset / not probed). */
#define B14369_NET_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14369_net_soft_ready(void)
{
	return B14369_NET_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_soft_ready_u_14369 - report soft net soft-ready aggregate lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the net soft-ready surface for wave 14361-14370.
 * Does not call libc or claim per-path ok lamps. No parent wires.
 */
uint32_t
gj_net_soft_ready_u_14369(void)
{
	(void)NULL;
	return b14369_net_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_soft_ready_u_14369(void)
    __attribute__((alias("gj_net_soft_ready_u_14369")));
