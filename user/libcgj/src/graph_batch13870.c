/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13870: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13870(void);
 *     - Returns the compile-time graph batch number for this TU (13870).
 *   uint32_t __gj_batch_id_13870  (alias)
 *   __libcgj_batch13870_marker = "libcgj-batch13870"
 *
 * Net/connectivity soft product CREATE-ONLY (13861-13870). Unique
 * gj_batch_id_13870 surface only; no multi-def. Distinct from
 * gj_net_ok_u_13861, gj_wifi_ok_u_13862, gj_ethernet_ok_u_13863,
 * gj_dns_ok_u_13864, gj_https_ok_u_13865, gj_cdn_ok_u_13866,
 * gj_steam_net_ok_u_13867, gj_offline_mode_ok_u_13868,
 * gj_net_soft_ready_u_13869, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13870_marker[] = "libcgj-batch13870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13870_id(void)
{
	return 13870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13870 - report this TU's graph batch number.
 *
 * Always returns 13870 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_13870(void)
{
	(void)NULL;
	return b13870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13870(void)
    __attribute__((alias("gj_batch_id_13870")));
