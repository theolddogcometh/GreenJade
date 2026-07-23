/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12870: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12870(void);
 *     - Returns the compile-time graph batch number for this TU (12870).
 *   uint32_t __gj_batch_id_12870  (alias)
 *   __libcgj_batch12870_marker = "libcgj-batch12870"
 *
 * Net/connectivity soft product CREATE-ONLY (12861-12870). Unique
 * gj_batch_id_12870 surface only; no multi-def. Distinct from
 * gj_batch_id_12670 (batch12670), gj_batch_id_12470 (batch12470),
 * gj_batch_id_12270 (batch12270), gj_net_ok_u_12861,
 * gj_wifi_ok_u_12862, gj_ethernet_ok_u_12863, gj_dns_ok_u_12864,
 * gj_https_ok_u_12865, gj_cdn_ok_u_12866, gj_steam_net_ok_u_12867,
 * gj_offline_mode_ok_u_12868, gj_net_soft_ready_u_12869, and prior
 * gj_batch_id / gj_batch_id_N symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12870_marker[] = "libcgj-batch12870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12870_id(void)
{
	return 12870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12870 - report this TU's graph batch number.
 *
 * Always returns 12870 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_12870(void)
{
	(void)NULL;
	return b12870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12870(void)
    __attribute__((alias("gj_batch_id_12870")));
